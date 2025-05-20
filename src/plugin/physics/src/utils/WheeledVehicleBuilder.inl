#include "WheeledVehicleBuilder.hpp"

#include "Object.hpp"
#include "OpenGL.hpp"
#include "RigidBody3D.hpp"

template <size_t WheelCount> ES::Engine::Entity ES::Plugin::Physics::Utils::WheeledVehicleBuilder<WheelCount>::Build()
{
    auto vehicleEntity = core.CreateEntity();

    if (!bodyMesh.has_value())
    {
        throw std::runtime_error("Body mesh not set");
    }

    vehicleEntity.AddComponent<ES::Plugin::Object::Component::Transform>(core, initialPosition);

    vehicleEntity.AddComponent<ES::Plugin::Object::Component::Mesh>(core, bodyMesh.value());

    // Create Jolt's mesh shape from the bodyMesh
    std::vector<JPH::Vec3> points;

    for (size_t i = 0; i < bodyMesh->vertices.size(); ++i)
    {
        points.emplace_back(JPH::Vec3(bodyMesh->vertices[i].x, bodyMesh->vertices[i].y, bodyMesh->vertices[i].z));
    }

    std::shared_ptr<JPH::ShapeSettings> bodySettings =
        std::make_shared<JPH::ConvexHullShapeSettings>(points.data(), points.size());
    bodySettings->SetEmbedded();

    // Init body settings to create the body shape
    std::shared_ptr<JPH::ShapeSettings> finalShapeSettings = std::make_shared<JPH::OffsetCenterOfMassShapeSettings>(
        JPH::Vec3(offsetCenterOfMassShape.x, offsetCenterOfMassShape.y, offsetCenterOfMassShape.z), bodySettings.get());
    finalShapeSettings->SetEmbedded();

    // Create a rigid body from the shape
    auto &vehicleRigidBody = vehicleEntity.AddComponent<ES::Plugin::Physics::Component::RigidBody3D>(
        core, finalShapeSettings, JPH::EMotionType::Dynamic, ES::Plugin::Physics::Utils::Layers::MOVING);

    // TODO: do not hardcode that
    vehicleEntity.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "no_light");
    vehicleEntity.AddComponent<ES::Plugin::OpenGL::Component::MaterialHandle>(core, "car_body");
    vehicleEntity.AddComponent<ES::Plugin::OpenGL::Component::ModelHandle>(core, "car_body");

    // Create the vehicle constraint settings
    auto vehicleConstraintSettings = std::make_shared<JPH::VehicleConstraintSettings>();
    vehicleConstraintSettings->SetEmbedded();

    vehicleConstraintSettings->mWheels.resize(WheelCount);

    for (size_t i = 0; i < WheelCount; ++i)
    {
        ES::Engine::Entity wheelEntity = core.CreateEntity();

        auto &wheel = wheelEntity.AddComponent<ES::Plugin::Physics::Component::WheeledVehicle3D::Wheel>(
            core, std::move(wheelSettings[i]));
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

    // TODO: do not hardcode controller settings
    static auto vehicleControllerSettings = std::make_shared<JPH::WheeledVehicleControllerSettings>();
    vehicleControllerSettings->SetEmbedded();
    vehicleControllerSettings->mEngine.mMaxTorque = 500.0f;
    vehicleControllerSettings->mTransmission.mClutchStrength = 10.0f;

    vehicleControllerSettings->mDifferentials.resize(differentialSettings.size());

    for (size_t i = 0; i < differentialSettings.size(); ++i)
    {
        vehicleControllerSettings->mDifferentials[i] = differentialSettings[i];
    }

    // Set the controller for the constraint
    vehicleConstraintSettings->mController = vehicleControllerSettings.get();

    // Add the component to the entity
    vehicleEntity.AddComponent<ES::Plugin::Physics::Component::WheeledVehicle3D>(core, bodySettings, finalShapeSettings,
                                                                                 vehicleConstraintSettings);

    return vehicleEntity;
}
