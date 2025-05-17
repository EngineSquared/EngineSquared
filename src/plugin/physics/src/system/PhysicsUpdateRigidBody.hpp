#pragma once

#include "Core.hpp"

namespace ES::Plugin::Physics::System {
/**
 * @brief Applies every update of the physics system.
 *
 * @param core  core
 * @note To be used with the "RelativeTimeUpdate" scheduler.
 */
void PhysicsUpdate(ES::Engine::Core &core);
void OnConstructLinkRigidBodiesToPhysicsSystem(ES::Engine::Core &core);
void SyncTransformsToRigidBodies(ES::Engine::Core &core);
void SyncRigidBodiesToTransforms(ES::Engine::Core &core);

// IMPORTANT: This function should only be used by OnConstructLinkRigidBodieToPhysicsSystem system.
void LinkRigidBodiesToPhysicsSystem(entt::registry &registry, entt::entity entity);
void UnlinkRigidBodiesToPhysicsSystem(entt::registry &registry, entt::entity entity);
} // namespace ES::Plugin::Physics::System
