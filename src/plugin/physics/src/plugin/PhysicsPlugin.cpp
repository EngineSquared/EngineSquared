#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"

#include "plugin/PhysicsPlugin.hpp"
#include "plugin/PluginEvent.hpp"

#include "system/ConstraintSystem.hpp"
#include "system/InitJoltPhysics.hpp"
#include "system/InitPhysicsManager.hpp"
#include "system/PhysicsUpdate.hpp"
#include "system/RigidBodySystem.hpp"
#include "system/SyncTransformSystem.hpp"

void Physics::Plugin::Bind()
{
    RequirePlugins<Event::Plugin>();

    RegisterSystems<Engine::Scheduler::Startup>(System::InitJoltPhysics);
    RegisterSystems<Engine::Scheduler::Startup>(System::InitPhysicsManager);
    RegisterSystems<Engine::Scheduler::Startup>(System::InitRigidBodySystem);
    RegisterSystems<Engine::Scheduler::Startup>(System::InitConstraintSystem);

    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(System::PhysicsUpdate);
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(System::SyncTransformWithPhysics);
}
