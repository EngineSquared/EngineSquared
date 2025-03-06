#include "PhysicsUpdate.hpp"

#include "PhysicsManager.hpp"
#include "RelativeTimeUpdate.hpp"

void ES::Plugin::Physics::System::PhysicsUpdate(ES::Engine::Core &registry)
{
    auto dt = registry.GetScheduler<ES::Engine::Scheduler::RelativeTimeUpdate>().GetCurrentDeltaTime();
    auto &physicsManager = registry.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();

    physicsManager.GetPhysicsSystem().Update(dt, 1, physicsManager.GetTempAllocator(), physicsManager.GetJobSystem());
}
