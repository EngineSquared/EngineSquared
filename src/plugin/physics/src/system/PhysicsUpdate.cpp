#include "PhysicsUpdate.hpp"

#include "Logger.hpp"
#include "PhysicsManager.hpp"
#include "RelativeTimeUpdate.hpp"
#include "RigidBody3D.hpp"
#include "Transform.hpp"

#include <fmt/format.h>

// TODO: find a way to have custom signal (so that we can send Core rather than entt::registry)
void ES::Plugin::Physics::System::LinkRigidBodiesToPhysicsSystem(entt::registry &registry, entt::entity entity)
{
    auto &rigidBody = registry.get<ES::Plugin::Physics::Component::RigidBody3D>(entity);
    if (rigidBody.body != nullptr)
    {
        return;
    }
    // TODO: have a RequireComponent function that does this
    if (!registry.all_of<ES::Plugin::Object::Component::Transform>(entity))
    {
        registry.emplace<ES::Plugin::Object::Component::Transform>(entity);
    }
    auto &transform = registry.get<ES::Plugin::Object::Component::Transform>(entity);

    // auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &physicsManager = registry.ctx().get<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &physicsSystem = physicsManager.GetPhysicsSystem();

    JPH::ShapeSettings::ShapeResult shape = rigidBody.shapeSettings->Create();

    if (shape.HasError())
    {
        ES::Utils::Log::Error(
            fmt::format("Failed to create shape for entity {}: {}", static_cast<uint32_t>(entity), shape.GetError()));
        return;
    }
    JPH::ShapeRefC shapeRef = shape.Get();

    JPH::BodyCreationSettings bodySettings(
        shapeRef, JPH::RVec3(transform.position.x, transform.position.y, transform.position.z),
        JPH::Quat(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w),
        rigidBody.motionType, rigidBody.layer);

    rigidBody.body = physicsSystem.GetBodyInterface().CreateBody(bodySettings);

    if (rigidBody.body == nullptr)
    {
        ES::Utils::Log::Error(
            fmt::format("Failed to create rigid body for entity {}: returned nullptr", static_cast<uint32_t>(entity)));
        return;
    }

    physicsSystem.GetBodyInterface().AddBody(
        rigidBody.body->GetID(), JPH::EActivation::Activate); // TODO: Not all of them should be activated by default
}

void ES::Plugin::Physics::System::UnlinkRigidBodiesToPhysicsSystem(entt::registry &registry, entt::entity entity)
{
    auto &rigidBody = registry.get<ES::Plugin::Physics::Component::RigidBody3D>(entity);
    if (rigidBody.body == nullptr)
    {
        return;
    }

    // auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &physicsManager = registry.ctx().get<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &physicsSystem = physicsManager.GetPhysicsSystem();

    physicsSystem.GetBodyInterface().RemoveBody(rigidBody.body->GetID());
    rigidBody.body = nullptr;
}

void ES::Plugin::Physics::System::OnConstructLinkRigidBodieToPhysicsSystem(ES::Engine::Core &core)
{
    core.GetRegistry()
        .on_construct<ES::Plugin::Physics::Component::RigidBody3D>()
        .connect<&ES::Plugin::Physics::System::LinkRigidBodiesToPhysicsSystem>();
    core.GetRegistry()
        .on_destroy<ES::Plugin::Physics::Component::RigidBody3D>()
        .connect<&ES::Plugin::Physics::System::UnlinkRigidBodiesToPhysicsSystem>();
}

void ES::Plugin::Physics::System::SyncTransformsToRigidBodies(ES::Engine::Core &core)
{
    core.GetRegistry()
        .view<ES::Plugin::Physics::Component::RigidBody3D, ES::Plugin::Object::Component::Transform>()
        .each([&](auto &rigidBody, auto &transform) {
            if (rigidBody.body != nullptr)
            {
                auto position = rigidBody.body->GetPosition();
                auto rotation = rigidBody.body->GetRotation();

                transform.position.x = position.GetX();
                transform.position.y = position.GetY();
                transform.position.z = position.GetZ();

                transform.rotation.w = rotation.GetW();
                transform.rotation.x = rotation.GetX();
                transform.rotation.y = rotation.GetY();
                transform.rotation.z = rotation.GetZ();
            }
        });
}

void ES::Plugin::Physics::System::SyncRigidBodiesToTransforms(ES::Engine::Core &core)
{
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyInterface();

    core.GetRegistry()
        .view<ES::Plugin::Physics::Component::RigidBody3D, ES::Plugin::Object::Component::Transform>()
        .each([&](auto &rigidBody, auto &transform) {
            if (rigidBody.body != nullptr)
            {
                bodyInterface.SetPositionAndRotationWhenChanged(
                    rigidBody.body->GetID(),
                    JPH::RVec3(transform.position.x, transform.position.y, transform.position.z),
                    JPH::Quat(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w),
                    JPH::EActivation::Activate // TODO: Not all of them should be activated by default
                );
            }
        });
}

void ES::Plugin::Physics::System::PhysicsUpdate(ES::Engine::Core &core)
{
    auto dt = core.GetScheduler<ES::Engine::Scheduler::RelativeTimeUpdate>().GetCurrentDeltaTime();
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    physicsManager.GetPhysicsSystem().Update(dt, physicsManager.GetCollisionSteps(), physicsManager.GetTempAllocator(),
                                             physicsManager.GetJobSystem());
}
