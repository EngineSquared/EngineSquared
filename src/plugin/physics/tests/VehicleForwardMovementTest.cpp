#include <gtest/gtest.h>

#include "builder/VehicleBuilder.hpp"
#include "component/BoxCollider.hpp"
#include "component/Mesh.hpp"
#include "component/RigidBody.hpp"
#include "component/Transform.hpp"
#include "component/Vehicle.hpp"
#include "component/VehicleController.hpp"
#include "component/VehicleInternal.hpp"
#include "component/WheelSettings.hpp"
#include "core/Core.hpp"
#include "helper/CreateShape.hpp"
#include "plugin/PhysicsPlugin.hpp"
#include "resource/Time.hpp"
#include "utils/ShapeGenerator.hpp"

#include <cmath>
#include <glm/glm.hpp>

/**
 * @brief Test that a vehicle moves forward when forward input is applied
 */
TEST(VehiclePlugin, VehicleForwardMovement)
{
    Engine::Core core;

    core.RegisterSystem<Engine::Scheduler::Update>(
        [&](Engine::Core &c) { c.GetResource<Engine::Resource::Time>()._elapsedTime = 0.016f; });

    core.AddPlugins<Physics::Plugin>();
    core.RunSystems(); // Initialize physics

    // Create floor
    auto floor =
        Object::Helper::CreatePlane(core, {.width = 50.0f, .depth = 50.0f, .position = glm::vec3(0.0f, 0.0f, 0.0f)});
    auto floorCollider = Physics::Component::BoxCollider(glm::vec3(25.0f, 0.5f, 25.0f));
    floor.AddComponent<Physics::Component::BoxCollider>(core, floorCollider);
    floor.AddComponent<Physics::Component::RigidBody>(core, Physics::Component::RigidBody::CreateStatic());

    // Create vehicle on floor
    Object::Component::Mesh chassisMesh = Object::Utils::GenerateCubeMesh(1.0f);
    Object::Component::Mesh wheelMesh = Object::Utils::GenerateWheelMesh(0.3f, 0.2f);

    // Configure wheel settings with good traction
    Physics::Component::WheelSettings frontWheel = Physics::Component::WheelSettings::CreateFrontWheel();
    frontWheel.radius = 0.3f;
    frontWheel.longitudinalFriction = 2.0f;
    frontWheel.lateralFriction = 2.0f;

    Physics::Component::WheelSettings rearWheel = Physics::Component::WheelSettings::CreateRearWheel();
    rearWheel.radius = 0.3f;
    rearWheel.longitudinalFriction = 2.0f;
    rearWheel.lateralFriction = 2.0f;

    Physics::Builder::VehicleBuilder<4> builder;
    auto vehicle = builder.SetChassisMesh(chassisMesh, glm::vec3(0.0f, 1.0f, 0.0f))
                       .SetWheelMesh(Physics::Component::WheelIndex::FrontLeft, wheelMesh)
                       .SetWheelMesh(Physics::Component::WheelIndex::FrontRight, wheelMesh)
                       .SetWheelMesh(Physics::Component::WheelIndex::RearLeft, wheelMesh)
                       .SetWheelMesh(Physics::Component::WheelIndex::RearRight, wheelMesh)
                       .SetWheelSettings(Physics::Component::WheelIndex::FrontLeft, frontWheel)
                       .SetWheelSettings(Physics::Component::WheelIndex::FrontRight, frontWheel)
                       .SetWheelSettings(Physics::Component::WheelIndex::RearLeft, rearWheel)
                       .SetWheelSettings(Physics::Component::WheelIndex::RearRight, rearWheel)
                       .SetDrivetrain(Physics::Component::DrivetrainType::RWD)
                       .SetChassisMass(1000.0f)
                       .Build(core);

    auto &registry = core.GetRegistry();

    // Let vehicle settle on ground
    for (int i = 0; i < 50; ++i)
    {
        core.RunSystems();
    }

    // Record starting position
    auto *transform = registry.try_get<Object::Component::Transform>(vehicle);
    ASSERT_NE(transform, nullptr);
    glm::vec3 startPos = transform->GetPosition();

    // Apply forward input
    auto *controller = registry.try_get<Physics::Component::VehicleController>(vehicle);
    ASSERT_NE(controller, nullptr);
    controller->SetForward(1.0f); // Full throttle

    // Run simulation with forward input
    for (int i = 0; i < 200; ++i)
    {
        core.RunSystems();
    }

    // Check final position
    glm::vec3 endPos = transform->GetPosition();
    glm::vec3 displacement = endPos - startPos;

    // Calculate horizontal movement (X/Z plane)
    float horizontalDistance = std::sqrt(displacement.x * displacement.x + displacement.z * displacement.z);

    EXPECT_GT(horizontalDistance, 3.0f) << "Vehicle should move at least 3 meters horizontally with full throttle";

    // Verify vehicle actually moved (not just vibrating in place)
    EXPECT_TRUE(std::abs(displacement.x) > 0.5f || std::abs(displacement.z) > 0.5f)
        << "Vehicle should have significant movement in X or Z direction";
}
