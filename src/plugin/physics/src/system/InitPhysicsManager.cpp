#include "Physics.pch.hpp"

#include "system/InitPhysicsManager.hpp"

#include "resource/PhysicsManager.hpp"

namespace Physics::System {
void InitPhysicsManager(Engine::Core &core)
{
    core.RegisterResource<Physics::Resource::PhysicsManager>(Physics::Resource::PhysicsManager())
        .Init(core);
}
} // namespace Physics::System
