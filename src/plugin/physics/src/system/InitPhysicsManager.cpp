#include "Physics.pch.hpp"

#include "system/InitPhysicsManager.hpp"

#include "resource/PhysicsManager.hpp"

namespace Plugin::Physics::System {
void InitPhysicsManager(Engine::Core &core)
{
    core.RegisterResource<Plugin::Physics::Resource::PhysicsManager>(Plugin::Physics::Resource::PhysicsManager())
        .Init(core);
}
} // namespace Plugin::Physics::System
