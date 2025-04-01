#include "PluginPhysics.hpp"
#include "InitJoltPhysics.hpp"
#include "InitPhysicsManager.hpp"
#include "PhysicsManager.hpp"
#include "PhysicsUpdate.hpp"
#include "RelativeTimeUpdate.hpp"
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

    RegisterSystems<ES::Engine::Scheduler::RelativeTimeUpdate>(
        ES::Plugin::Physics::System::SyncRigidBodiesToTransforms);
    RegisterSystems<ES::Engine::Scheduler::RelativeTimeUpdate>(ES::Plugin::Physics::System::PhysicsUpdate);
    RegisterSystems<ES::Engine::Scheduler::RelativeTimeUpdate>(
        ES::Plugin::Physics::System::SyncTransformsToRigidBodies);
    RegisterSystems<ES::Engine::Scheduler::RelativeTimeUpdate>(ES::Plugin::Physics::System::SyncSoftBodiesData);

    RegisterSystems<ES::Engine::Scheduler::Shutdown>(ES::Plugin::Physics::System::ShutdownJoltPhysics);
}
