#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"

#include "plugin/PluginPhysics.hpp"

#include "system/InitJoltPhysics.hpp"
#include "system/InitPhysicsManager.hpp"
#include "system/PhysicsUpdate.hpp"
#include "system/PhysicsUpdateRigidBody.hpp"
#include "system/PhysicsUpdateSoftBody.hpp"
#include "system/PhysicsUpdateWheeledVehicle.hpp"
#include "system/ShutdownJoltPhysics.hpp"

void ES::Plugin::Physics::Plugin::Bind()
{
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Physics::System::InitJoltPhysics);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Physics::System::InitPhysicsManager);

    RegisterSystems<ES::Engine::Scheduler::Startup>(
        ES::Plugin::Physics::System::OnConstructLinkRigidBodiesToPhysicsSystem);
    RegisterSystems<ES::Engine::Scheduler::Startup>(
        ES::Plugin::Physics::System::OnConstructLinkSoftBodiesToPhysicsSystem);
    RegisterSystems<ES::Engine::Scheduler::Startup>(
        ES::Plugin::Physics::System::OnConstructLinkWheeledVehiclesToPhysicsSystem);

    RegisterSystems<ES::Engine::Scheduler::FixedTimeUpdate>(
        ES::Plugin::Physics::System::SyncRigidBodiesToTransforms, ES::Plugin::Physics::System::PhysicsUpdate,
        ES::Plugin::Physics::System::SyncTransformsToRigidBodies, ES::Plugin::Physics::System::SyncSoftBodiesData,
        ES::Plugin::Physics::System::SyncWheeledVehicleWheels);

    RegisterSystems<ES::Engine::Scheduler::Shutdown>(ES::Plugin::Physics::System::ShutdownJoltPhysics);
}
