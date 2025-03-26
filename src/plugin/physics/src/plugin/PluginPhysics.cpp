#include "PluginPhysics.hpp"
#include "PhysicsManager.hpp"
#include "PhysicsUpdate.hpp"
#include "Startup.hpp"

void ES::Plugin::Physics::Plugin::Bind()
{
    RegisterSystems<ES::Engine::Scheduler::Startup>([](const ES::Engine::Core &) {
        JPH::RegisterDefaultAllocator();
    });
    RegisterSystems<ES::Engine::Scheduler::Startup>([](const ES::Engine::Core &) {
        JPH::Factory::sInstance = new JPH::Factory();
    });
    RegisterSystems<ES::Engine::Scheduler::Startup>([](const ES::Engine::Core &) {
        JPH::RegisterTypes();
    });
    RegisterSystems<ES::Engine::Scheduler::Startup>([](ES::Engine::Core &core) {
        core.RegisterResource<ES::Plugin::Physics::Resource::PhysicsManager>(ES::Plugin::Physics::Resource::PhysicsManager())
            .Init();
    });
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Physics::System::OnConstructLinkRigidBodieToPhysicsSystem);
    
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Physics::System::SyncRigidBodiesToTransforms);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Physics::System::PhysicsUpdate);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Physics::System::SyncTransformsToRigidBodies);
    
    RegisterSystems<ES::Engine::Scheduler::Shutdown>([](const ES::Engine::Core &) {
        JPH::UnregisterTypes();
    });
    RegisterSystems<ES::Engine::Scheduler::Shutdown>([](const ES::Engine::Core &) {
        delete JPH::Factory::sInstance;
        JPH::Factory::sInstance = nullptr;
    });
}
