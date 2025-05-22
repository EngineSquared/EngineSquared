#include "JoltPhysics.pch.hpp"

#include "PhysicsUpdateWheeledVehicle.hpp"

#include "FixedTimeUpdate.hpp"
#include "Logger.hpp"
#include "Mesh.hpp"
#include "PhysicsManager.hpp"
#include "RigidBody3D.hpp"
#include "Transform.hpp"
#include "WheeledVehicle3D.hpp"

#include <fmt/format.h>

// TODO: find a way to have custom signal (so that we can send Core rather than entt::registry)
void ES::Plugin::Physics::System::LinkWheeledVehicleToPhysicsSystem(entt::registry &registry, entt::entity entity)
{
    auto &wheeledVehicle = registry.get<ES::Plugin::Physics::Component::WheeledVehicle3D>(entity);
    if (wheeledVehicle.bodySettings == nullptr || wheeledVehicle.finalShapeSettings == nullptr ||
        wheeledVehicle.vehicleConstraintSettings == nullptr)
    {
        ES::Utils::Log::Error(fmt::format("WheeledVehicle3D component is not fully initialized for entity {}",
                                          static_cast<uint32_t>(entity)));
        return;
    }
    auto &rigidBody = registry.get<ES::Plugin::Physics::Component::RigidBody3D>(entity);
    if (rigidBody.body == nullptr)
    {
        ES::Utils::Log::Error(
            fmt::format("WheeledVehicle3D component is trying to link to an entity that has no RigidBody3D component"));
        return;
    }

    // Build the vehicle constraint
    wheeledVehicle.vehicleConstraint =
        std::make_shared<JPH::VehicleConstraint>(*rigidBody.body, *wheeledVehicle.vehicleConstraintSettings.get());
    wheeledVehicle.vehicleConstraint->SetEmbedded();

    wheeledVehicle.vehicleConstraintSettings = nullptr;

    wheeledVehicle.vehicleConstraint->SetVehicleCollisionTester(wheeledVehicle.collisionTester.get());

    auto &physicsManager = registry.ctx().get<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &physicsSystem = physicsManager.GetPhysicsSystem();

    physicsSystem.AddConstraint(wheeledVehicle.vehicleConstraint.get());
    physicsSystem.AddStepListener(wheeledVehicle.vehicleConstraint.get());
}

void ES::Plugin::Physics::System::UnlinkWheeledVehicleToPhysicsSystem(entt::registry &registry, entt::entity entity)
{
    auto &wheeledVehicle = registry.get<ES::Plugin::Physics::Component::WheeledVehicle3D>(entity);
    if (wheeledVehicle.vehicleConstraint == nullptr)
    {
        ES::Utils::Log::Error(fmt::format("WheeledVehicle3D component is not fully initialized for entity {}",
                                          static_cast<uint32_t>(entity)));
        return;
    }

    auto &physicsManager = registry.ctx().get<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &physicsSystem = physicsManager.GetPhysicsSystem();

    physicsSystem.RemoveConstraint(wheeledVehicle.vehicleConstraint.get());
    physicsSystem.RemoveStepListener(wheeledVehicle.vehicleConstraint.get());
}

void ES::Plugin::Physics::System::OnConstructLinkWheeledVehiclesToPhysicsSystem(ES::Engine::Core &core)
{
    core.GetRegistry()
        .on_construct<ES::Plugin::Physics::Component::WheeledVehicle3D>()
        .connect<&ES::Plugin::Physics::System::LinkWheeledVehicleToPhysicsSystem>();
    core.GetRegistry()
        .on_destroy<ES::Plugin::Physics::Component::WheeledVehicle3D>()
        .connect<&ES::Plugin::Physics::System::UnlinkWheeledVehicleToPhysicsSystem>();
}

void ES::Plugin::Physics::System::SyncWheeledVehicleWheels(ES::Engine::Core &core)
{
    core.GetRegistry()
        .view<ES::Plugin::Physics::Component::WheeledVehicle3D::Wheel, ES::Plugin::Object::Component::Transform>()
        .each([&](auto, auto &wheel, auto &transform) {
            if (wheel.wheelSettings == nullptr)
            {
                return;
            }

            if (!wheel.parentVehicle.template HasComponents<ES::Plugin::Physics::Component::WheeledVehicle3D>(core))
            {
                ES::Utils::Log::Error(fmt::format(
                    "SyncWheeledVehicleWheels: WheeledVehicle3D component is not fully initialized for entity {}",
                    static_cast<uint32_t>(wheel.parentVehicle)));
                return;
            }

            // TODO: do not hardcode this
            static const JPH::Vec3 wheelRight(1.0f, 0.0f, 0.0f);
            static const JPH::Vec3 wheelUp(0.0f, 1.0f, 0.0f);

            auto &wheeledVehicle =
                wheel.parentVehicle.template GetComponents<ES::Plugin::Physics::Component::WheeledVehicle3D>(core);

            auto wTransform =
                wheeledVehicle.vehicleConstraint->GetWheelWorldTransform(wheel.index, wheelRight, wheelUp);
            auto wPosition = wTransform.GetTranslation();
            auto wRotation = wTransform.GetRotation().GetQuaternion();

            transform.setPosition(glm::vec3(wPosition.GetX(), wPosition.GetY(), wPosition.GetZ()));

            transform.rotation.w = wRotation.GetW();
            transform.rotation.x = wRotation.GetX();
            transform.rotation.y = wRotation.GetY();
            transform.rotation.z = wRotation.GetZ();
        });
}
