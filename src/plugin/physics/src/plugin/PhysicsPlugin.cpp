#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"

#include "plugin/PluginPhysics.hpp"

#include "system/InitJoltPhysics.hpp"
#include "system/InitPhysicsManager.hpp"

void Plugin::Physics::Plugin::Bind()
{
    RegisterSystems<Engine::Scheduler::Startup>(Plugin::Physics::System::InitJoltPhysics);
    RegisterSystems<Engine::Scheduler::Startup>(Plugin::Physics::System::InitPhysicsManager);
}
