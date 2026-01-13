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
#include "component/BoxCollider.hpp"
#include "component/CapsuleCollider.hpp"
#include "component/SoftBody.hpp"
#include "component/SoftBodyInternal.hpp"
#include "component/SphereCollider.hpp"
#include "resource/PhysicsManager.hpp"
#include "utils/JoltConversions.hpp"
#include "utils/Layers.hpp"
#include <fmt/format.h>
#include <set>
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
 * @brief Generate edge constraints from triangle face indices
 *
 * This function takes triangle indices and creates unique edge pairs
 * for soft body constraint creation.
 *
 * @param faceIndices Triangle indices (size must be multiple of 3)
 * @return Vector of unique edge pairs (vertex index A, vertex index B)
 */
static std::vector<std::pair<uint32_t, uint32_t>> GenerateEdgesFromFaces(const std::vector<uint32_t> &faceIndices)
{
    if (faceIndices.empty())
        return {};

    std::set<std::pair<uint32_t, uint32_t>> edgeSet;
    for (size_t i = 0u; i + 2u < faceIndices.size(); i += 3u)
    {
        uint32_t v0 = faceIndices[i];
        uint32_t v1 = faceIndices[i + 1u];
        uint32_t v2 = faceIndices[i + 2u];

        auto addEdge = [&edgeSet](uint32_t a, uint32_t b) {
            if (a > b)
                std::swap(a, b);
            edgeSet.insert({a, b});
        };

        addEdge(v0, v1);
        addEdge(v1, v2);
        addEdge(v2, v0);
    }

    std::vector<std::pair<uint32_t, uint32_t>> edges;
    edges.reserve(edgeSet.size());
    for (const auto &edge : edgeSet)
    {
        edges.push_back(edge);
    }
    return edges;
}

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
 * @param scale The scale factor from Transform to apply to vertices (default: 1.0)
 * @return CreateSettingsResult with settings and vertex mapping
 *
 * @note The scale is applied to mesh vertices before creating the Jolt soft body.
 *       This allows users to use Transform.scale to resize soft bodies without
 *       manually scaling the mesh vertices.
 */
static CreateSettingsResult CreateJoltSharedSettings(const Component::SoftBody &softBody,
                                                     const Object::Component::Mesh &mesh,
                                                     const glm::vec3 &scale = glm::vec3(1.0f))
{
    CreateSettingsResult result;
    auto settings = new JPH::SoftBodySharedSettings();

    // Deduplicate the mesh to get unique vertices and proper indices
    // This is necessary because OBJLoader creates "flat" meshes with duplicated vertices
    DeduplicatedMesh deduped = DeduplicateMesh(mesh);

    // Store the vertex map for later sync
    result.vertexMap = std::move(deduped.vertexMap);

    Log::Info(fmt::format("SoftBody mesh: original {} vertices -> {} unique vertices, {} indices (scale: {:.2f}, "
                          "{:.2f}, {:.2f})",
                          mesh.vertices.size(), deduped.vertices.size(), deduped.indices.size(), scale.x, scale.y,
                          scale.z));

    // Add unique vertices with scale applied
    settings->mVertices.reserve(deduped.vertices.size());
    for (size_t i = 0; i < deduped.vertices.size(); ++i)
    {
        JPH::SoftBodySharedSettings::Vertex v;
        // Apply scale to vertex positions
        glm::vec3 scaledPos = deduped.vertices[i] * scale;
        v.mPosition = JPH::Float3(scaledPos.x, scaledPos.y, scaledPos.z);
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

        // Warn if Collider components are present (they are ignored for SoftBody)
        if (registry.any_of<Component::BoxCollider, Component::SphereCollider, Component::CapsuleCollider>(entity))
        {
            Log::Warn("SoftBody: Collider components (BoxCollider, SphereCollider, CapsuleCollider) are ignored "
                      "for soft bodies. Use SoftBodySettings::vertexRadius for collision detection.");
        }

        //=====================================================================
        // Auto-initialization: Initialize invMasses and edges from Mesh
        //=====================================================================
        if (softBody.invMasses.empty())
        {
            // Auto-initialize invMasses from mesh vertex count
            softBody.invMasses.resize(mesh->vertices.size(), 1.0f);
            Log::Debug(fmt::format("SoftBody: Auto-initialized {} invMasses from Mesh", mesh->vertices.size()));
        }
        else if (softBody.invMasses.size() != mesh->vertices.size())
        {
            Log::Error(fmt::format("SoftBody: invMasses size ({}) doesn't match Mesh vertices size ({})",
                                   softBody.invMasses.size(), mesh->vertices.size()));
            return;
        }

        // Auto-generate edges from faces if not provided
        if (softBody.edges.empty() && !mesh->indices.empty())
        {
            if (mesh->indices.size() % 3 != 0)
            {
                Log::Error("SoftBody: Mesh indices malformed (not multiple of 3)");
                return;
            }
            softBody.edges = GenerateEdgesFromFaces(mesh->indices);
            Log::Debug(fmt::format("SoftBody: Auto-generated {} edges from Mesh faces", softBody.edges.size()));
        }

        // Apply pinned vertices (set invMass = 0 for pinned)
        for (uint32_t pinnedIdx : softBody.pinnedVertices)
        {
            if (pinnedIdx < softBody.invMasses.size())
            {
                softBody.invMasses[pinnedIdx] = 0.0f;
            }
        }

        // Get position, rotation, and scale from Transform if available
        JPH::RVec3 position = JPH::RVec3::sZero();
        JPH::Quat rotation = JPH::Quat::sIdentity();
        glm::vec3 scale = glm::vec3(1.0f);

        if (auto *transform = registry.try_get<Object::Component::Transform>(entity))
        {
            position = Utils::ToJoltRVec3(transform->GetPosition());
            rotation = Utils::ToJoltQuat(transform->GetRotation());
            scale = transform->GetScale();
        }

        // Create shared settings from Mesh (includes deduplication, vertex mapping, and scale application)
        auto settingsResult = CreateJoltSharedSettings(softBody, *mesh, scale);

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

        // Store internal component with vertex mapping AND initial scale for sync
        // The initial scale is needed to convert Jolt vertices back to mesh local space
        registry.emplace<Component::SoftBodyInternal>(entity, bodyID, std::move(settingsResult.vertexMap), scale);

        Log::Info(fmt::format(
            "Created SoftBody for entity {} with {} vertices, {} faces at position ({:.2f}, {:.2f}, {:.2f}), scale "
            "({:.2f}, {:.2f}, {:.2f})",
            static_cast<uint32_t>(entity), mesh->vertices.size(), mesh->indices.size() / 3, position.GetX(),
            position.GetY(), position.GetZ(), scale.x, scale.y, scale.z));
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
        // Jolt vertices are in WORLD scale (scaled by initialScale during creation)
        // We need to convert back to LOCAL mesh space by dividing by initialScale
        // Then the GPU will apply Transform.scale (which should match initialScale)
        const glm::vec3 &invScale = glm::vec3(1.0f) / internal.initialScale;

        if (!internal.vertexMap.empty())
        {
            // Use vertex map: original mesh vertices are mapped to deduplicated Jolt vertices
            for (size_t origIdx = 0; origIdx < mesh->vertices.size() && origIdx < internal.vertexMap.size(); ++origIdx)
            {
                uint32_t joltIdx = internal.vertexMap[origIdx];
                if (joltIdx < joltVertices.size())
                {
                    const auto &v = joltVertices[joltIdx];
                    // Convert from Jolt world-scale space back to mesh local space
                    glm::vec3 worldPos(v.mPosition.GetX(), v.mPosition.GetY(), v.mPosition.GetZ());
                    mesh->vertices[origIdx] = worldPos * invScale;
                }
            }
        }
        else
        {
            // Fallback: direct 1:1 mapping (for procedurally generated meshes)
            for (size_t i = 0; i < joltVertices.size() && i < mesh->vertices.size(); ++i)
            {
                const auto &v = joltVertices[i];
                // Convert from Jolt world-scale space back to mesh local space
                glm::vec3 worldPos(v.mPosition.GetX(), v.mPosition.GetY(), v.mPosition.GetZ());
                mesh->vertices[i] = worldPos * invScale;
            }
        }
    }
}

} // namespace Physics::System
