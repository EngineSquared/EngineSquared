#pragma once

#include "core/Core.hpp"

namespace ES::Plugin::Physics::System {
void OnConstructLinkRigidBodiesToPhysicsSystem(ES::Engine::Core &core);
void SyncTransformsToRigidBodies(ES::Engine::Core &core);
void SyncRigidBodiesToTransforms(ES::Engine::Core &core);

// IMPORTANT: This function should only be used by OnConstructLinkRigidBodieToPhysicsSystem system.
void LinkRigidBodiesToPhysicsSystem(entt::registry &registry, entt::entity entity);
void UnlinkRigidBodiesToPhysicsSystem(entt::registry &registry, entt::entity entity);
} // namespace ES::Plugin::Physics::System
