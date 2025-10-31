#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"

#include "plugin/PhysicsPlugin.hpp"

#include "system/InitJoltPhysics.hpp"
#include "system/InitPhysicsManager.hpp"
#include "system/PhysicsUpdate.hpp"
#include "system/RigidBodySystem.hpp"

void Physics::Plugin::Bind()
{
    RegisterSystems<Engine::Scheduler::Startup>(System::InitJoltPhysics);
    RegisterSystems<Engine::Scheduler::Startup>(System::InitPhysicsManager);
    RegisterSystems<Engine::Scheduler::Startup>(System::InitRigidBodySystem);

    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(System::PhysicsUpdate);
}
