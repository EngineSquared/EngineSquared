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
#include <unordered_map>

#include "Object.hpp"

// Jolt soft body includes
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodyCreationSettings.h>
#include <Jolt/Physics/SoftBody/SoftBodyMotionProperties.h>
#include <Jolt/Physics/SoftBody/SoftBodySharedSettings.h>

namespace Physics::System {

//=============================================================================
// Helper Functions
//=============================================================================

/**
 * @brief Hash function for glm::vec3 to use in unordered_map
 */
struct Vec3Hash {
    size_t operator()(const glm::vec3 &v) const
    {
        // Use a simple hash combining x, y, z components
        size_t h1 = std::hash<float>{}(v.x);
        size_t h2 = std::hash<float>{}(v.y);
        size_t h3 = std::hash<float>{}(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

/**
 * @brief Equality function for glm::vec3 with epsilon tolerance
 */
struct Vec3Equal {
    bool operator()(const glm::vec3 &a, const glm::vec3 &b) const { return a.x == b.x && a.y == b.y && a.z == b.z; }
};

/**
 * @brief Structure holding deduplicated mesh data for Jolt soft body
 */
struct DeduplicatedMesh {
    std::vector<glm::vec3> vertices; ///< Unique vertices
    std::vector<uint32_t> indices;   ///< Face indices referencing unique vertices
    std::vector<uint32_t> vertexMap; ///< Maps original vertex index to deduplicated index
};

/**
 * @brief Convert a potentially "flat" mesh (with duplicated vertices) to an indexed mesh with unique vertices
 *
 * OBJLoader creates meshes where each face has its own copy of vertices, resulting in
 * indices like [0, 1, 2, 3, 4, 5, ...] where vertices are duplicated.
 * Jolt SoftBody needs a proper indexed mesh with shared vertices for constraint creation.
 *
 * @param mesh The input mesh (potentially with duplicated vertices)
 * @return DeduplicatedMesh with unique vertices and proper indices
 */
static DeduplicatedMesh DeduplicateMesh(const Object::Component::Mesh &mesh)
{
    DeduplicatedMesh result;
    result.vertexMap.resize(mesh.vertices.size());

    std::unordered_map<glm::vec3, uint32_t, Vec3Hash, Vec3Equal> vertexToIndex;

    for (size_t i = 0; i < mesh.vertices.size(); ++i)
    {
        const auto &vertex = mesh.vertices[i];

        auto it = vertexToIndex.find(vertex);
        if (it != vertexToIndex.end())
        {
            // Vertex already exists, reuse its index
            result.vertexMap[i] = it->second;
        }
        else
        {
            // New unique vertex
            uint32_t newIndex = static_cast<uint32_t>(result.vertices.size());
            result.vertices.push_back(vertex);
            vertexToIndex[vertex] = newIndex;
            result.vertexMap[i] = newIndex;
        }
    }

    // Remap indices to point to deduplicated vertices
    result.indices.reserve(mesh.indices.size());
    for (uint32_t idx : mesh.indices)
    {
        if (idx < result.vertexMap.size())
        {
            result.indices.push_back(result.vertexMap[idx]);
        }
    }

    return result;
}

/**
 * @brief Result of creating Jolt shared settings, includes vertex mapping
 */
struct CreateSettingsResult {
    JPH::Ref<JPH::SoftBodySharedSettings> settings;
    std::vector<uint32_t> vertexMap; ///< Maps original vertex index to deduplicated index
};

/**
 * @brief Convert SoftBody component to Jolt SoftBodySharedSettings
 * @param softBody The soft body component with settings and physics data
 * @param mesh The mesh containing vertices and face indices
 * @return CreateSettingsResult with settings and vertex mapping
 */
static CreateSettingsResult CreateJoltSharedSettings(const Component::SoftBody &softBody,
                                                     const Object::Component::Mesh &mesh)
{
    CreateSettingsResult result;
    auto settings = new JPH::SoftBodySharedSettings();

    // Deduplicate the mesh to get unique vertices and proper indices
    // This is necessary because OBJLoader creates "flat" meshes with duplicated vertices
    DeduplicatedMesh deduped = DeduplicateMesh(mesh);

    // Store the vertex map for later sync
    result.vertexMap = std::move(deduped.vertexMap);

    Log::Info(fmt::format("SoftBody mesh: original {} vertices -> {} unique vertices, {} indices", mesh.vertices.size(),
                          deduped.vertices.size(), deduped.indices.size()));

    // Add unique vertices
    settings->mVertices.reserve(deduped.vertices.size());
    for (size_t i = 0; i < deduped.vertices.size(); ++i)
    {
        JPH::SoftBodySharedSettings::Vertex v;
        v.mPosition = JPH::Float3(deduped.vertices[i].x, deduped.vertices[i].y, deduped.vertices[i].z);
        v.mVelocity = JPH::Float3(0, 0, 0);

        // Map inverse mass from original vertices - find any original vertex that maps to this one
        float invMass = 1.0f;
        for (size_t origIdx = 0; origIdx < result.vertexMap.size(); ++origIdx)
        {
            if (result.vertexMap[origIdx] == i && origIdx < softBody.invMasses.size())
            {
                invMass = softBody.invMasses[origIdx];
                break;
            }
        }
        v.mInvMass = invMass;

        settings->mVertices.emplace_back(v);
    }

    // Add faces using deduplicated indices
    if (!deduped.indices.empty())
    {
        settings->mFaces.reserve(deduped.indices.size() / 3);
        for (size_t i = 0; i + 2 < deduped.indices.size(); i += 3)
        {
            uint32_t idx0 = deduped.indices[i];
            uint32_t idx1 = deduped.indices[i + 1];
            uint32_t idx2 = deduped.indices[i + 2];

            // Skip degenerate faces (faces where two or more vertices are the same)
            if (idx0 == idx1 || idx1 == idx2 || idx0 == idx2)
            {
                continue;
            }

            // Validate indices are within bounds
            if (idx0 >= deduped.vertices.size() || idx1 >= deduped.vertices.size() || idx2 >= deduped.vertices.size())
            {
                Log::Warn(
                    fmt::format("SoftBody: Skipping face with out-of-bounds indices ({}, {}, {})", idx0, idx1, idx2));
                continue;
            }

            settings->mFaces.emplace_back(JPH::SoftBodySharedSettings::Face(idx0, idx1, idx2, 0));
        }

        Log::Info(fmt::format("SoftBody: Created {} faces", settings->mFaces.size()));

        // Only create constraints if we have valid faces
        if (!settings->mFaces.empty())
        {
            // Create constraints automatically based on faces
            JPH::SoftBodySharedSettings::VertexAttributes attributes(
                softBody.settings.edgeCompliance, softBody.settings.shearCompliance, softBody.settings.bendCompliance);

            settings->CreateConstraints(&attributes, 1, JPH::SoftBodySharedSettings::EBendType::Distance);
        }
    }
    else if (!softBody.edges.empty())
    {
        // For rope/chain without faces, use edge constraints from SoftBody
        settings->mEdgeConstraints.reserve(softBody.edges.size());
        for (const auto &edge : softBody.edges)
        {
            settings->mEdgeConstraints.emplace_back(
                JPH::SoftBodySharedSettings::Edge(edge.first, edge.second, softBody.settings.edgeCompliance));
        }
    }

    // Optimize for parallel simulation
    settings->Optimize();

    result.settings = settings;
    return result;
}

/**
 * @brief Create Jolt SoftBodyCreationSettings from component
 */
static JPH::SoftBodyCreationSettings
CreateJoltCreationSettings(const Component::SoftBody &softBody,
                           const JPH::Ref<JPH::SoftBodySharedSettings> &sharedSettings, const JPH::RVec3 &position,
                           const JPH::Quat &rotation)
{
    JPH::SoftBodyCreationSettings creationSettings(sharedSettings, position, rotation, Utils::Layers::MOVING);

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

        // SoftBody requires a Mesh component for geometry
        auto *mesh = registry.try_get<Object::Component::Mesh>(entity);
        if (!mesh)
        {
            Log::Error("SoftBody: No Mesh component found. Add Mesh component before SoftBody.");
            return;
        }

        if (mesh->vertices.empty())
        {
            Log::Error("SoftBody: Mesh has no vertices");
            return;
        }

        // Verify invMasses matches Mesh vertex count
        if (softBody.invMasses.size() != mesh->vertices.size())
        {
            Log::Error(fmt::format("SoftBody: invMasses size ({}) doesn't match Mesh vertices size ({})",
                                   softBody.invMasses.size(), mesh->vertices.size()));
            return;
        }

        // Validate mesh indices
        if (mesh->indices.size() % 3 != 0)
        {
            Log::Error("SoftBody: Mesh indices malformed (not multiple of 3)");
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

        // Create shared settings from Mesh (includes deduplication and vertex mapping)
        auto settingsResult = CreateJoltSharedSettings(softBody, *mesh);

        // Create body settings
        auto creationSettings = CreateJoltCreationSettings(softBody, settingsResult.settings, position, rotation);

        // Create the soft body
        auto &bodyInterface = physicsManager.GetBodyInterface();
        JPH::Body *body = bodyInterface.CreateSoftBody(creationSettings);

        if (!body)
        {
            Log::Error("SoftBody: Failed to create Jolt soft body");
            return;
        }

        JPH::BodyID bodyID = body->GetID();

        // Add to physics system (activate)
        bodyInterface.AddBody(bodyID, JPH::EActivation::Activate);

        // Store internal component with vertex mapping for sync
        registry.emplace<Component::SoftBodyInternal>(entity, bodyID, std::move(settingsResult.vertexMap));

        Log::Info(fmt::format(
            "Created SoftBody for entity {} with {} vertices, {} faces at position ({:.2f}, {:.2f}, {:.2f})",
            static_cast<uint32_t>(entity), mesh->vertices.size(), mesh->indices.size() / 3, position.GetX(),
            position.GetY(), position.GetZ()));
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

        auto &bodyInterface = physicsManager.GetBodyInterface();
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

    if (!registry.ctx().contains<Engine::Core *>())
        registry.ctx().emplace<Engine::Core *>(&core);

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

    auto &bodyInterface = physicsManager.GetBodyInterface();

    // Update vertex positions from Jolt simulation
    auto view = registry.view<Component::SoftBody, Component::SoftBodyInternal>();

    for (auto entity : view)
    {
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
        const auto &joltVertices = motionProps->GetVertices();

        // Write to Mesh.vertices (used by graphics plugin for rendering)
        auto *mesh = registry.try_get<Object::Component::Mesh>(entity);
        if (!mesh || mesh->vertices.empty())
            continue;

        // Update Transform with center of mass position (like RigidBody sync)
        // This way the GPU applies the Transform, and vertices stay in local space
        auto *transform = registry.try_get<Object::Component::Transform>(entity);
        if (transform)
        {
            transform->SetPosition(Utils::FromJoltRVec3(bodyPosition));
            // Note: Soft bodies don't have a single rotation, so we leave it as-is
        }

        // Use the vertex map to sync deduplicated Jolt vertices back to the original mesh
        // Vertices are written in LOCAL space (relative to center of mass)
        // The Transform handles world positioning
        if (!internal.vertexMap.empty())
        {
            // Use vertex map: original mesh vertices are mapped to deduplicated Jolt vertices
            for (size_t origIdx = 0; origIdx < mesh->vertices.size() && origIdx < internal.vertexMap.size(); ++origIdx)
            {
                uint32_t joltIdx = internal.vertexMap[origIdx];
                if (joltIdx < joltVertices.size())
                {
                    const auto &v = joltVertices[joltIdx];
                    // Local space only - Transform handles world position
                    mesh->vertices[origIdx] = glm::vec3(v.mPosition.GetX(), v.mPosition.GetY(), v.mPosition.GetZ());
                }
            }
        }
        else
        {
            // Fallback: direct 1:1 mapping (for procedurally generated meshes)
            for (size_t i = 0; i < joltVertices.size() && i < mesh->vertices.size(); ++i)
            {
                const auto &v = joltVertices[i];
                // Local space only - Transform handles world position
                mesh->vertices[i] = glm::vec3(v.mPosition.GetX(), v.mPosition.GetY(), v.mPosition.GetZ());
            }
        }
    }
}

} // namespace Physics::System
