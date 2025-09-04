#pragma once

#include "core/Core.hpp"

namespace ES::Plugin::Physics::System {

void OnConstructLinkSoftBodiesToPhysicsSystem(ES::Engine::Core &core);
void SyncSoftBodiesData(ES::Engine::Core &core);

// IMPORTANT: This function should only be used by OnConstructLinkRigidBodieToPhysicsSystem system
void LinkSoftBodiesToPhysicsSystem(entt::registry &registry, entt::entity entity);
void UnlinkSoftBodiesToPhysicsSystem(entt::registry &registry, entt::entity entity);
} // namespace ES::Plugin::Physics::System
