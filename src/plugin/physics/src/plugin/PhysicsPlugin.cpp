#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"

#include "plugin/PhysicsPlugin.hpp"
#include "plugin/PluginEvent.hpp"

#include "resource/VehicleTelemetry.hpp"
#include "system/ColliderSystem.hpp"
#include "system/ConstraintSystem.hpp"
#include "system/InitJoltPhysics.hpp"
#include "system/InitPhysicsManager.hpp"
#include "system/PhysicsUpdate.hpp"
#include "system/RigidBodySystem.hpp"
#include "system/SoftBodyAttachmentSync.hpp"
#include "system/SoftBodySystem.hpp"
#include "system/SyncTransformSystem.hpp"
#include "system/VehicleControlSystem.hpp"
#include "system/VehicleRPMUpdate.hpp"
#include "system/VehicleSystem.hpp"
#include "system/WheelTransformSyncSystem.hpp"

void Physics::Plugin::Bind()
{
    RequirePlugins<Event::Plugin>();

    RegisterResource<Resource::VehicleTelemetry>(Resource::VehicleTelemetry{});

    RegisterSystems<Engine::Scheduler::Startup>(System::InitJoltPhysics);
    RegisterSystems<Engine::Scheduler::Startup>(System::InitPhysicsManager);
    RegisterSystems<Engine::Scheduler::Startup>(System::InitRigidBodySystem);
    // RegisterSystems<Engine::Scheduler::Startup>(System::InitColliderSystem);
    RegisterSystems<Engine::Scheduler::Startup>(System::InitConstraintSystem);
    RegisterSystems<Engine::Scheduler::Startup>(System::InitSoftBodySystem);
    RegisterSystems<Engine::Scheduler::Startup>(System::InitSoftBodyAttachmentSync);
    RegisterSystems<Engine::Scheduler::Startup>(System::InitVehicleSystem);

    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(System::PhysicsUpdate);
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(System::VehicleControlSystem);
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(System::VehicleRPMUpdate);
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(System::SyncSoftBodyAttachments); // Before vertex sync
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(System::SyncTransformWithPhysics);
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(System::SyncSoftBodyVertices);
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(System::WheelTransformSyncSystem);

    RegisterSystems<Engine::Scheduler::Shutdown>(System::ShutdownSoftBodySystem);
}
