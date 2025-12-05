/**************************************************************************
 * EngineSquared v0.1.1
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
 * @file SoftBodySystem.cpp
 * @brief Soft body system implementation
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-12-05
 **************************************************************************/

#include "Physics.pch.hpp"

#include "system/SoftBodySystem.hpp"

#include "Logger.hpp"
#include "component/SoftBody.hpp"
#include "component/SoftBodyInternal.hpp"
#include "resource/PhysicsManager.hpp"
#include "utils/JoltConversions.hpp"
#include "utils/Layers.hpp"
#include <fmt/format.h>

#include "Object.hpp"

// Jolt soft body includes
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodySharedSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodyMotionProperties.h>

namespace Physics::System {

//=============================================================================
// Helper Functions
//=============================================================================

/**
 * @brief Convert SoftBody component to Jolt SoftBodySharedSettings
 */
static JPH::Ref<JPH::SoftBodySharedSettings> CreateJoltSharedSettings(const Component::SoftBody &softBody)
{
    auto settings = new JPH::SoftBodySharedSettings();

    // Add vertices
    settings->mVertices.reserve(softBody.vertices.size());
    for (size_t i = 0; i < softBody.vertices.size(); ++i)
    {
        JPH::SoftBodySharedSettings::Vertex v;
        v.mPosition = JPH::Float3(softBody.vertices[i].x, softBody.vertices[i].y, softBody.vertices[i].z);
        v.mVelocity = JPH::Float3(0, 0, 0);
        v.mInvMass = softBody.invMasses[i];
        settings->mVertices.push_back(v);
    }

    // Add faces (if any)
    if (!softBody.faces.empty())
    {
        settings->mFaces.reserve(softBody.faces.size() / 3);
        for (size_t i = 0; i < softBody.faces.size(); i += 3)
        {
            settings->mFaces.push_back(JPH::SoftBodySharedSettings::Face(
                softBody.faces[i],
                softBody.faces[i + 1],
                softBody.faces[i + 2],
                0  // Material index
            ));
        }

        // Create constraints automatically based on faces
        JPH::SoftBodySharedSettings::VertexAttributes attributes(
            softBody.settings.edgeCompliance,
            softBody.settings.shearCompliance,
            softBody.settings.bendCompliance);

        settings->CreateConstraints(&attributes, 1, JPH::SoftBodySharedSettings::EBendType::Distance);
    }
    else if (!softBody.edges.empty())
    {
        // For rope/chain without faces, manually add edge constraints
        for (const auto &edge : softBody.edges)
        {
            settings->mEdgeConstraints.push_back(JPH::SoftBodySharedSettings::Edge(
                edge.first,
                edge.second,
                softBody.settings.edgeCompliance));
        }
    }

    // Optimize for parallel simulation
    settings->Optimize();

    return settings;
}

/**
 * @brief Create Jolt SoftBodyCreationSettings from component
 */
static JPH::SoftBodyCreationSettings CreateJoltCreationSettings(
    const Component::SoftBody &softBody,
    const JPH::Ref<JPH::SoftBodySharedSettings> &sharedSettings,
    const JPH::RVec3 &position,
    const JPH::Quat &rotation)
{
    JPH::SoftBodyCreationSettings creationSettings(
        sharedSettings,
        position,
        rotation,
        Utils::Layers::MOVING);

    // Apply settings
    creationSettings.mNumIterations = softBody.settings.solverIterations;
    creationSettings.mLinearDamping = softBody.settings.linearDamping;
    creationSettings.mMaxLinearVelocity = softBody.settings.maxLinearVelocity;
    creationSettings.mRestitution = softBody.settings.restitution;
    creationSettings.mFriction = softBody.settings.friction;
    creationSettings.mPressure = softBody.settings.pressure;
    creationSettings.mGravityFactor = softBody.settings.gravityFactor;
    creationSettings.mVertexRadius = softBody.settings.vertexRadius;
    creationSettings.mAllowSleeping = softBody.settings.allowSleeping;
    creationSettings.mFacesDoubleSided = softBody.settings.doubleSidedFaces;

    return creationSettings;
}

//=============================================================================
// Soft Body Lifecycle Handlers
//=============================================================================

static void OnSoftBodyConstruct(entt::registry &registry, entt::entity entity)
{
    try
    {
        auto &core = *registry.ctx().get<Engine::Core *>();

        auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
        if (!physicsManager.IsPhysicsActivated())
        {
            Log::Error("Cannot create SoftBody: Physics system not activated");
            return;
        }

        auto &softBody = registry.get<Component::SoftBody>(entity);

        // Validate soft body
        if (!softBody.IsValid())
        {
            Log::Error("SoftBody: Invalid configuration (no vertices or malformed faces)");
            return;
        }

        // Get position from Transform if available
        JPH::RVec3 position = JPH::RVec3::sZero();
        JPH::Quat rotation = JPH::Quat::sIdentity();

        if (auto *transform = registry.try_get<Object::Component::Transform>(entity))
        {
            position = Utils::ToJoltRVec3(transform->GetPosition());
            rotation = Utils::ToJoltQuat(transform->GetRotation());
        }

        // Create shared settings (can be shared between multiple instances)
        auto sharedSettings = CreateJoltSharedSettings(softBody);

        // Create body settings
        auto creationSettings = CreateJoltCreationSettings(softBody, sharedSettings, position, rotation);

        // Create the soft body
        auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyInterface();
        JPH::Body *body = bodyInterface.CreateSoftBody(creationSettings);

        if (!body)
        {
            Log::Error("SoftBody: Failed to create Jolt soft body");
            return;
        }

        JPH::BodyID bodyID = body->GetID();

        // Add to physics system (activate)
        bodyInterface.AddBody(bodyID, JPH::EActivation::Activate);

        // Store internal component
        registry.emplace<Component::SoftBodyInternal>(entity, bodyID);

        Log::Debug(fmt::format("Created SoftBody for entity {} with {} vertices, {} faces at position ({:.2f}, {:.2f}, {:.2f})",
                               static_cast<uint32_t>(entity),
                               softBody.GetVertexCount(),
                               softBody.GetFaceCount(),
                               position.GetX(), position.GetY(), position.GetZ()));
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("SoftBody error: {}", e.what()));
    }
}

static void OnSoftBodyDestroy(entt::registry &registry, entt::entity entity)
{
    try
    {
        auto &core = *registry.ctx().get<Engine::Core *>();

        auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
        if (!physicsManager.IsPhysicsActivated())
            return;

        auto *internal = registry.try_get<Component::SoftBodyInternal>(entity);
        if (!internal || !internal->IsValid())
            return;

        auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyInterface();
        bodyInterface.RemoveBody(internal->bodyID);
        bodyInterface.DestroyBody(internal->bodyID);

        Log::Debug(fmt::format("Destroyed SoftBody for entity {}", static_cast<uint32_t>(entity)));

        registry.remove<Component::SoftBodyInternal>(entity);
    }
    catch (const std::exception &e)
    {
        Log::Error(fmt::format("SoftBody destroy error: {}", e.what()));
    }
}

//=============================================================================
// Public System Functions
//=============================================================================

void InitSoftBodySystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    // Ensure core pointer is in context
    if (!registry.ctx().contains<Engine::Core *>())
    {
        registry.ctx().emplace<Engine::Core *>(&core);
    }

    // Register SoftBody hooks
    registry.on_construct<Component::SoftBody>().connect<&OnSoftBodyConstruct>();
    registry.on_destroy<Component::SoftBody>().connect<&OnSoftBodyDestroy>();

    Log::Info("SoftBodySystem initialized");
}

void ShutdownSoftBodySystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    // Disconnect hooks
    registry.on_construct<Component::SoftBody>().disconnect<&OnSoftBodyConstruct>();
    registry.on_destroy<Component::SoftBody>().disconnect<&OnSoftBodyDestroy>();

    Log::Info("SoftBodySystem shutdown");
}

void SyncSoftBodyVertices(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
    if (!physicsManager.IsPhysicsActivated())
        return;

    auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyInterface();

    // Update vertex positions from Jolt simulation
    auto view = registry.view<Component::SoftBody, Component::SoftBodyInternal>();

    for (auto entity : view)
    {
        auto &softBody = view.get<Component::SoftBody>(entity);
        auto &internal = view.get<Component::SoftBodyInternal>(entity);

        if (!internal.IsValid())
            continue;

        // Get soft body motion properties
        JPH::BodyLockRead lock(physicsManager.GetPhysicsSystem().GetBodyLockInterface(), internal.bodyID);
        if (!lock.Succeeded())
            continue;

        const JPH::Body &body = lock.GetBody();
        if (!body.IsSoftBody())
            continue;

        const JPH::SoftBodyMotionProperties *motionProps =
            static_cast<const JPH::SoftBodyMotionProperties *>(body.GetMotionProperties());

        // Get body center of mass in world space
        JPH::RVec3 bodyPosition = body.GetCenterOfMassPosition();

        // Update vertex positions (vertices are in local space relative to body center of mass)
        const auto &vertices = motionProps->GetVertices();

        for (size_t i = 0; i < vertices.size() && i < softBody.vertices.size(); ++i)
        {
            const auto &v = vertices[i];
            // Convert from local to world space by adding body position
            softBody.vertices[i] = glm::vec3(
                bodyPosition.GetX() + v.mPosition.GetX(),
                bodyPosition.GetY() + v.mPosition.GetY(),
                bodyPosition.GetZ() + v.mPosition.GetZ());
        }
    }
}

} // namespace Physics::System
