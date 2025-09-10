#include "WheeledVehicleBuilder.hpp"

#include "Object.hpp"
#include "OpenGL.hpp"
#include "component/RigidBody3D.hpp"

template <size_t WheelCount> ES::Engine::Entity ES::Plugin::Physics::Utils::WheeledVehicleBuilder<WheelCount>::Build()
{
    auto vehicleEntity = core.CreateEntity();

    if (!bodyMesh.has_value())
    {
        throw std::runtime_error("Body mesh not set");
    }

    vehicleEntity.template AddComponent<ES::Plugin::Object::Component::Transform>(core, initialPosition);

    vehicleEntity.template AddComponent<ES::Plugin::Object::Component::Mesh>(core, bodyMesh.value());

    // Create Jolt's mesh shape from the bodyMesh
    std::vector<JPH::Vec3> points;

    for (size_t i = 0; i < bodyMesh->vertices.size(); ++i)
    {
        points.emplace_back(JPH::Vec3(bodyMesh->vertices[i].x, bodyMesh->vertices[i].y, bodyMesh->vertices[i].z));
    }

    std::shared_ptr<JPH::ConvexHullShapeSettings> bodySettings =
        std::make_shared<JPH::ConvexHullShapeSettings>(points.data(), points.size());
    bodySettings->SetEmbedded();

    // Init body settings to create the body shape
    std::shared_ptr<JPH::ShapeSettings> finalShapeSettings = std::make_shared<JPH::OffsetCenterOfMassShapeSettings>(
        JPH::Vec3(offsetCenterOfMassShape.x, offsetCenterOfMassShape.y, offsetCenterOfMassShape.z), bodySettings.get());
    finalShapeSettings->SetEmbedded();

    // Create a rigid body from the shape
    auto &vehicleRigidBody = vehicleEntity.template AddComponent<ES::Plugin::Physics::Component::RigidBody3D>(
        core, finalShapeSettings, JPH::EMotionType::Dynamic, ES::Plugin::Physics::Utils::Layers::MOVING, false,
        [this](JPH::BodyCreationSettings &bodySettings) {
            bodySettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
            bodySettings.mMassPropertiesOverride.mMass = vehicleMass;
        });

    // Create the vehicle constraint settings
    auto vehicleConstraintSettings = std::make_shared<JPH::VehicleConstraintSettings>();
    vehicleConstraintSettings->SetEmbedded();

    vehicleConstraintSettings->mWheels.resize(WheelCount);

    for (size_t i = 0; i < WheelCount; ++i)
    {
        ES::Engine::Entity wheelEntity = core.CreateEntity();

        glm::vec3 wheelPosition = wheelOffsets[i];

        wheelSettings[i]->mPosition = JPH::Vec3(wheelPosition.x, wheelPosition.y, wheelPosition.z);

        wheelEntity.template AddComponent<ES::Plugin::Object::Component::Transform>(core, wheelPosition);
        wheelEntity.AddComponent<ES::Plugin::Object::Component::Mesh>(core, wheelMesh.value());

        if (wheelCallbackFn)
        {
            wheelCallbackFn(core, wheelEntity);
        }

        auto &wheel = wheelEntity.AddComponent<ES::Plugin::Physics::Component::WheeledVehicle3D::Wheel>(
            core, vehicleEntity, std::move(wheelSettings[i]), i);
        vehicleConstraintSettings->mWheels[i] = wheel.wheelSettings.get();
    }

    // Apply the function to modify the constraint settings
    if (constraintSettingsFn)
    {
        constraintSettingsFn(*vehicleConstraintSettings);
    }

    vehicleConstraintSettings->mAntiRollBars.resize(antiRollBars.size());

    for (size_t i = 0; i < antiRollBars.size(); ++i)
    {
        vehicleConstraintSettings->mAntiRollBars[i] = antiRollBars[i];
    }

    auto vehicleControllerSettings = std::make_shared<JPH::WheeledVehicleControllerSettings>();
    vehicleControllerSettings->SetEmbedded();

    vehicleControllerSettings->mDifferentials.resize(differentialSettings.size());

    for (size_t i = 0; i < differentialSettings.size(); ++i)
    {
        vehicleControllerSettings->mDifferentials[i] = differentialSettings[i];
    }

    if (vehicleControllerSettingsFn)
    {
        vehicleControllerSettingsFn(*vehicleControllerSettings);
    }

    // Set the controller for the constraint
    vehicleConstraintSettings->mController = vehicleControllerSettings.get();

    // Add the component to the entity
    vehicleEntity.AddComponent<ES::Plugin::Physics::Component::WheeledVehicle3D>(
        core, bodySettings, finalShapeSettings, vehicleConstraintSettings, vehicleControllerSettings, collisionTester);

    if (vehicleCallbackFn)
    {
        vehicleCallbackFn(core, vehicleEntity);
    }

    return vehicleEntity;
}
