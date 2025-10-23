#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"

#include "plugin/PhysicsPlugin.hpp"

#include "system/InitJoltPhysics.hpp"
#include "system/InitPhysicsManager.hpp"
#include "system/PhysicsUpdate.hpp"

void Physics::Plugin::Bind()
{
    RegisterSystems<Engine::Scheduler::Startup>(System::InitJoltPhysics);
    RegisterSystems<Engine::Scheduler::Startup>(System::InitPhysicsManager);

    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(System::PhysicsUpdate);
}
