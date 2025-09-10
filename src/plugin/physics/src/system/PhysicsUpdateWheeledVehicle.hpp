#pragma once

#include "core/Core.hpp"

namespace ES::Plugin::Physics::System {
void OnConstructLinkWheeledVehiclesToPhysicsSystem(ES::Engine::Core &core);

// IMPORTANT: This function should only be used by OnConstructLinkWheeledVehicleToPhysicsSystem system.
void LinkWheeledVehicleToPhysicsSystem(entt::registry &registry, entt::entity entity);
void UnlinkWheeledVehicleToPhysicsSystem(entt::registry &registry, entt::entity entity);

void SyncWheeledVehicleWheels(ES::Engine::Core &core);
} // namespace ES::Plugin::Physics::System
