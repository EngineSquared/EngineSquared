/**************************************************************************
 * EngineSquared v0.2.0
 *
 * EngineSquared is a software package, part of the Engine² organization.
 *
 * This file is part of the EngineSquared project that is under MIT License.
 * Copyright © 2025-present by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the MIT License. See the project's LICENSE file for
 * the full license text and details.
 *
 * @file SoftBodyAttachmentSync.cpp
 * @brief System for synchronizing SoftBody attachments to parent RigidBodies
 *
 * @author @EngineSquared
 * @version 0.2.0
 * @date 2026-01-25
 **************************************************************************/

#include "Physics.pch.hpp"

#include "system/SoftBodyAttachmentSync.hpp"

#include "Logger.hpp"
#include "component/RigidBody.hpp"
#include "component/RigidBodyInternal.hpp"
#include "component/SoftBody.hpp"
#include "component/SoftBodyAttachment.hpp"
#include "component/SoftBodyInternal.hpp"
#include "resource/PhysicsManager.hpp"
#include "utils/JoltConversions.hpp"
#include <fmt/format.h>

#include "Object.hpp"

#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/SoftBody/SoftBodyMotionProperties.h>

namespace Physics::System {

//=============================================================================
// Helper Functions
//=============================================================================

/**
 * @brief Get the world transform of a RigidBody entity
 * @param core Engine core reference
 * @param entity Entity with RigidBody
 * @param outPosition Output world position
 * @param outRotation Output world rotation
 * @return true if transform was obtained successfully
 */
static bool GetRigidBodyWorldTransform(Engine::Core &core, Engine::Entity entity, glm::vec3 &outPosition,
                                       glm::quat &outRotation)
{
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();

    auto *rbInternal = entity.TryGetComponent<Component::RigidBodyInternal>();
    if (!rbInternal || !rbInternal->IsValid())
    {
        // Fallback to Transform component if no physics body
        auto *transform = entity.TryGetComponent<Object::Component::Transform>();
        if (!transform)
            return false;

        outPosition = transform->GetPosition();
        outRotation = transform->GetRotation();
        return true;
    }

    // Get position and rotation from Jolt body
    auto &bodyInterface = physicsManager.GetBodyInterface();
    JPH::RVec3 joltPos;
    JPH::Quat joltRot;
    bodyInterface.GetPositionAndRotation(rbInternal->bodyID, joltPos, joltRot);

    outPosition = Utils::FromJoltRVec3(joltPos);
    outRotation = Utils::FromJoltQuat(joltRot);

    return true;
}

/**
 * @brief Initialize anchor local positions from current mesh vertices
 * @param attachment The attachment component to initialize
 * @param mesh The mesh containing vertex data
 */
static void InitializeAnchorPositions(Component::SoftBodyAttachment &attachment, const Object::Component::Mesh &mesh)
{
    const auto &vertices = mesh.GetVertices();

    attachment.anchorLocalPositions.clear();
    attachment.anchorLocalPositions.reserve(attachment.anchorVertices.size());

    for (uint32_t vertexIdx : attachment.anchorVertices)
    {
        if (vertexIdx < vertices.size())
        {
            attachment.anchorLocalPositions.push_back(vertices[vertexIdx]);
        }
        else
        {
            Log::Warn(fmt::format("SoftBodyAttachment: Anchor vertex index {} out of bounds (mesh has {} vertices)",
                                  vertexIdx, vertices.size()));
            attachment.anchorLocalPositions.push_back(glm::vec3(0.0f));
        }
    }

    attachment.initialized = true;
    Log::Debug(fmt::format("SoftBodyAttachment: Initialized {} anchor positions", attachment.anchorLocalPositions.size()));
}

//=============================================================================
// Public System Functions
//=============================================================================

void InitSoftBodyAttachmentSync(Engine::Core &core)
{
    // Currently no hooks needed - sync is done in update
    // Could add on_construct hook here if needed for validation
    Log::Debug("SoftBodyAttachmentSync initialized");
}

void SyncSoftBodyAttachments(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
    if (!physicsManager.IsPhysicsActivated())
        return;

    auto &bodyInterface = physicsManager.GetBodyInterface();

    // Iterate all entities with SoftBody + SoftBodyAttachment
    auto view = registry.view<Component::SoftBody, Component::SoftBodyInternal, Component::SoftBodyAttachment>();

    for (auto entityId : view)
    {
        Engine::Entity entity{core, entityId};

        auto &attachment = view.get<Component::SoftBodyAttachment>(entityId);
        auto &softBodyInternal = view.get<Component::SoftBodyInternal>(entityId);

        // Validate attachment
        if (!attachment.IsValid())
            continue;

        // Get parent entity - check if it exists in registry
        if (!core.GetRegistry().valid(attachment.parentEntity))
        {
            Log::Warn(fmt::format("SoftBodyAttachment: Parent entity {} is invalid",
                                  static_cast<uint32_t>(attachment.parentEntity)));
            continue;
        }
        Engine::Entity parentEntity{core, attachment.parentEntity};

        // Get parent world transform
        glm::vec3 parentPos;
        glm::quat parentRot;
        if (!GetRigidBodyWorldTransform(core, parentEntity, parentPos, parentRot))
        {
            continue;
        }

        // Get the mesh for this soft body
        auto *mesh = entity.TryGetComponent<Object::Component::Mesh>();
        if (!mesh)
            continue;

        // Initialize anchor local positions on first update
        if (!attachment.initialized)
        {
            InitializeAnchorPositions(attachment, *mesh);
        }

        // Skip if no anchors
        if (attachment.anchorVertices.empty())
            continue;

        // Get soft body motion properties to modify vertices
        JPH::BodyLockWrite lock(physicsManager.GetPhysicsSystem().GetBodyLockInterface(), softBodyInternal.bodyID);
        if (!lock.Succeeded())
            continue;

        JPH::Body &body = lock.GetBody();
        if (!body.IsSoftBody())
            continue;

        JPH::SoftBodyMotionProperties *motionProps =
            static_cast<JPH::SoftBodyMotionProperties *>(body.GetMotionProperties());

        auto &joltVertices = motionProps->GetVertices();

        // Get scale from internal (used when creating the soft body)
        const glm::vec3 &scale = softBodyInternal.initialScale;

        // Calculate the soft body center position in world space
        glm::vec3 softBodyWorldPos = parentPos + parentRot * attachment.localOffset;
        glm::quat softBodyWorldRot = attachment.syncRotation ? parentRot * attachment.localRotation : attachment.localRotation;

        // First, update the soft body's world transform (center of mass position)
        // This is critical - it sets where the soft body exists in world space
        body.SetPositionAndRotationInternal(Utils::ToJoltRVec3(softBodyWorldPos),
                                            Utils::ToJoltQuat(softBodyWorldRot));

        // Now update anchor vertices
        // IMPORTANT: Jolt SoftBody vertices are relative to the body's center of mass!
        // So we need to compute local positions, not world positions
        for (size_t i = 0; i < attachment.anchorVertices.size() && i < attachment.anchorLocalPositions.size(); ++i)
        {
            uint32_t meshVertexIdx = attachment.anchorVertices[i];

            // Map mesh vertex to Jolt vertex
            uint32_t joltVertexIdx = meshVertexIdx;
            if (!softBodyInternal.vertexMap.empty() && meshVertexIdx < softBodyInternal.vertexMap.size())
            {
                joltVertexIdx = softBodyInternal.vertexMap[meshVertexIdx];
            }

            if (joltVertexIdx >= joltVertices.size())
                continue;

            // The anchor's local position in mesh space (scaled)
            // These positions are already relative to mesh origin, which should match body COM
            glm::vec3 localPos = attachment.anchorLocalPositions[i] * scale;

            // Apply anchor strength (interpolate between current and target)
            if (attachment.anchorStrength < 1.0f)
            {
                glm::vec3 currentPos(joltVertices[joltVertexIdx].mPosition.GetX(),
                                     joltVertices[joltVertexIdx].mPosition.GetY(),
                                     joltVertices[joltVertexIdx].mPosition.GetZ());
                localPos = glm::mix(currentPos, localPos, attachment.anchorStrength);
            }

            // Set position in Jolt (relative to body COM - this is LOCAL space)
            joltVertices[joltVertexIdx].mPosition = JPH::Vec3(localPos.x, localPos.y, localPos.z);

            // Zero out velocity for anchored vertices to prevent oscillation
            joltVertices[joltVertexIdx].mVelocity = JPH::Vec3::sZero();
        }
    }
}

} // namespace Physics::System
