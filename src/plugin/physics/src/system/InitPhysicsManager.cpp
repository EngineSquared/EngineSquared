#include "JoltPhysics.pch.hpp"

#include "InitPhysicsManager.hpp"

#include "PhysicsManager.hpp"

namespace ES::Plugin::Physics::System {
void InitPhysicsManager(ES::Engine::Core &core)
{
    core.RegisterResource<ES::Plugin::Physics::Resource::PhysicsManager>(
            ES::Plugin::Physics::Resource::PhysicsManager())
        .Init(core);
}
} // namespace ES::Plugin::Physics::System
