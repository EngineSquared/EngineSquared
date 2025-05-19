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

    auto &vehicle = vehicleEntity.AddComponent<Component::WheeledVehicle3D>(core);

    vehicleEntity.AddComponent<ES::Plugin::Object::Component::Transform>(core, initialPosition);

    vehicleEntity.AddComponent<ES::Plugin::Object::Component::Mesh>(core, bodyMesh.value());

    // Create Jolt's mesh shape from the bodyMesh
    std::vector<JPH::Vec3> points;

    for (size_t i = 0; i < bodyMesh->vertices.size(); ++i)
    {
        points.emplace_back(
            JPH::Vec3(bodyMesh->vertices[i].x, bodyMesh->vertices[i].y, bodyMesh->vertices[i].z));
    }

    vehicle.bodySettings = std::make_shared<JPH::ConvexHullShapeSettings>(points.data(), points.size());
    vehicle.bodySettings->SetEmbedded();

    // Init body settings to create the body shape
    vehicle.finalShapeSettings = std::make_shared<JPH::OffsetCenterOfMassShapeSettings>(
        JPH::Vec3(offsetCenterOfMassShape.x, offsetCenterOfMassShape.y, offsetCenterOfMassShape.z),
        vehicle.bodySettings.get());

    // Create a rigid body from the shape
    vehicleEntity.AddComponent<ES::Plugin::Physics::Component::RigidBody3D>(
        core, vehicle.finalShapeSettings, JPH::EMotionType::Dynamic, ES::Plugin::Physics::Utils::Layers::MOVING);

    // TODO: do not hardcode that
    vehicleEntity.AddComponent<ES::Plugin::OpenGL::Component::ShaderHandle>(core, "no_light");
    vehicleEntity.AddComponent<ES::Plugin::OpenGL::Component::MaterialHandle>(core, "car_body");
    vehicleEntity.AddComponent<ES::Plugin::OpenGL::Component::ModelHandle>(core, "car_body");

    // Create the vehicle constraint
    vehicle.vehicleConstraint = std::make_shared<JPH::VehicleConstraintSettings>();
    vehicle.vehicleConstraint->SetEmbedded();

    // Apply the function to modify the constraint settings
    if (constraintSettingsFn)
    {
        constraintSettingsFn(*vehicle.vehicleConstraint);
    }

    return vehicleEntity;
}
