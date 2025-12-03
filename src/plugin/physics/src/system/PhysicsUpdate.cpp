#include "Physics.pch.hpp"

#include "system/PhysicsUpdate.hpp"

#include "Logger.hpp"
#include "resource/PhysicsManager.hpp"
#include "scheduler/FixedTimeUpdate.hpp"

#include <fmt/format.h>

void Physics::System::PhysicsUpdate(Engine::Core &core)
{
    auto &physicsManager = core.GetResource<Physics::Resource::PhysicsManager>();
    if (!physicsManager.IsPhysicsActivated())
        return;

    auto dt = core.GetScheduler<Engine::Scheduler::FixedTimeUpdate>().GetTickRate();

    physicsManager.GetPhysicsSystem().Update(dt, physicsManager.GetCollisionSteps(), physicsManager.GetTempAllocator(),
                                             physicsManager.GetJobSystem());

    if (auto contactListener = physicsManager.GetContactListener())
        contactListener->ProcessBufferedEvents(core);
}
