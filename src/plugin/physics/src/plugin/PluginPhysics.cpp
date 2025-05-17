#include "PluginPhysics.hpp"
#include "FixedTimeUpdate.hpp"
#include "InitJoltPhysics.hpp"
#include "InitPhysicsManager.hpp"
#include "PhysicsManager.hpp"
#include "PhysicsUpdate.hpp"
#include "PhysicsUpdateRigidBody.hpp"
#include "PhysicsUpdateSoftBody.hpp"
#include "ShutdownJoltPhysics.hpp"
#include "Startup.hpp"

void ES::Plugin::Physics::Plugin::Bind()
{
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Physics::System::InitJoltPhysics);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Physics::System::InitPhysicsManager);

    RegisterSystems<ES::Engine::Scheduler::Startup>(
        ES::Plugin::Physics::System::OnConstructLinkRigidBodiesToPhysicsSystem);
    RegisterSystems<ES::Engine::Scheduler::Startup>(
        ES::Plugin::Physics::System::OnConstructLinkSoftBodiesToPhysicsSystem);

    RegisterSystems<ES::Engine::Scheduler::FixedTimeUpdate>(
        ES::Plugin::Physics::System::SyncRigidBodiesToTransforms, ES::Plugin::Physics::System::PhysicsUpdate,
        ES::Plugin::Physics::System::SyncTransformsToRigidBodies, ES::Plugin::Physics::System::SyncSoftBodiesData);

    RegisterSystems<ES::Engine::Scheduler::Shutdown>(ES::Plugin::Physics::System::ShutdownJoltPhysics);
}
