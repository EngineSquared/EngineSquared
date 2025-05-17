#include "PhysicsUpdate.hpp"

#include "FixedTimeUpdate.hpp"
#include "Logger.hpp"
#include "PhysicsManager.hpp"

#include <fmt/format.h>

void ES::Plugin::Physics::System::PhysicsUpdate(ES::Engine::Core &core)
{
    auto dt = core.GetScheduler<ES::Engine::Scheduler::FixedTimeUpdate>().GetTickRate();
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    physicsManager.GetPhysicsSystem().Update(dt, physicsManager.GetCollisionSteps(), physicsManager.GetTempAllocator(),
                                             physicsManager.GetJobSystem());
}