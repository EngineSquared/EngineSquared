#include "Physics.pch.hpp"

#include "system/ColliderSystem.hpp"

#include "Logger.hpp"
#include "component/BoxCollider.hpp"
#include "component/CapsuleCollider.hpp"
#include "component/MeshCollider.hpp"
#include "component/RigidBody.hpp"
#include "component/SoftBody.hpp"
#include "component/SphereCollider.hpp"
#include "entity/EntityId.hpp"
#include <fmt/format.h>

namespace Physics::System {

//=============================================================================
// Internal helper
//=============================================================================

/**
 * @brief Adds a static RigidBody to an entity if it doesn't already have one
 *
 * @param registry The entt registry
 * @param entity The entity to check and potentially add RigidBody to
 * @param colliderTypeName Name of the collider for logging purposes
 */
static void EnsureStaticRigidBody(Engine::Core::Registry &registry, Engine::EntityId entity,
                                   const char *colliderTypeName)
{
    // If entity already has a RigidBody OR a SoftBody, do nothing
    if (registry.try_get<Component::RigidBody>(entity) != nullptr || registry.try_get<Component::SoftBody>(entity) != nullptr)
        return;

    // Add a static RigidBody by default
    auto staticRigidBody = Component::RigidBody::CreateStatic();
    registry.emplace<Component::RigidBody>(entity, staticRigidBody);

    Log::Debug(fmt::format("{} added to entity {} without RigidBody - added static RigidBody", colliderTypeName,
                           static_cast<uint32_t>(entity)));
}

//=============================================================================
// Collider hooks
//=============================================================================

static void OnBoxColliderConstruct(Engine::Core::Registry &registry, Engine::EntityId entity)
{
    EnsureStaticRigidBody(registry, entity, "BoxCollider");
}

static void OnSphereColliderConstruct(Engine::Core::Registry &registry, Engine::EntityId entity)
{
    EnsureStaticRigidBody(registry, entity, "SphereCollider");
}

static void OnCapsuleColliderConstruct(Engine::Core::Registry &registry, Engine::EntityId entity)
{
    EnsureStaticRigidBody(registry, entity, "CapsuleCollider");
}

static void OnMeshColliderConstruct(Engine::Core::Registry &registry, Engine::EntityId entity)
{
    EnsureStaticRigidBody(registry, entity, "MeshCollider");
}

//=============================================================================
// Public System Function
//=============================================================================

void InitColliderSystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    // Ensure Core pointer is in context (may already be set by RigidBodySystem)
    if (!registry.ctx().contains<Engine::Core *>())
        registry.ctx().emplace<Engine::Core *>(&core);

    // Register hooks for all collider types
    registry.on_construct<Component::BoxCollider>().connect<&OnBoxColliderConstruct>();
    registry.on_construct<Component::SphereCollider>().connect<&OnSphereColliderConstruct>();
    registry.on_construct<Component::CapsuleCollider>().connect<&OnCapsuleColliderConstruct>();
    registry.on_construct<Component::MeshCollider>().connect<&OnMeshColliderConstruct>();

    Log::Debug("ColliderSystem initialized - colliders without RigidBody will get static RigidBody");
}

} // namespace Physics::System
