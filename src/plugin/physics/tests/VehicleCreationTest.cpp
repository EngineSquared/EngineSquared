#include <gtest/gtest.h>

#include "builder/VehicleBuilder.hpp"
#include "component/Mesh.hpp"
#include "component/RigidBody.hpp"
#include "component/Transform.hpp"
#include "component/Vehicle.hpp"
#include "component/VehicleController.hpp"
#include "component/VehicleInternal.hpp"
#include "core/Core.hpp"
#include "plugin/PhysicsPlugin.hpp"
#include "resource/Time.hpp"
#include "scheduler/Startup.hpp"
#include "utils/ShapeGenerator.hpp"

#include <glm/glm.hpp>

/**
 * @brief Test that a vehicle can be created with VehicleBuilder
 */
TEST(VehiclePlugin, VehicleCreation)
{
    Engine::Core core;
    core.AddPlugins<Physics::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Update>(
        [&](Engine::Core &c) { c.GetResource<Engine::Resource::Time>()._elapsedTime = 0.016f; });

    // Run startup systems to initialize physics
    core.GetScheduler<Engine::Scheduler::Startup>().RunSystems();

    // Create simple meshes for chassis and wheels
    Object::Component::Mesh chassisMesh = Object::Utils::GenerateCubeMesh(1.0f);
    Object::Component::Mesh wheelMesh = Object::Utils::GenerateWheelMesh(0.3f, 0.2f);

    // Build vehicle
    Physics::Builder::VehicleBuilder<4> builder;
    auto vehicle = builder.SetChassisMesh(chassisMesh, glm::vec3(0.0f, 5.0f, 0.0f))
                       .SetWheelMesh(Physics::Component::WheelIndex::FrontLeft, wheelMesh)
                       .SetWheelMesh(Physics::Component::WheelIndex::FrontRight, wheelMesh)
                       .SetWheelMesh(Physics::Component::WheelIndex::RearLeft, wheelMesh)
                       .SetWheelMesh(Physics::Component::WheelIndex::RearRight, wheelMesh)
                       .SetDrivetrain(Physics::Component::DrivetrainType::RWD)
                       .Build(core);

    ASSERT_TRUE(vehicle.IsAlive());

    // Verify components exist
    auto &registry = core.GetRegistry();
    EXPECT_TRUE(vehicle.HasComponents<Physics::Component::Vehicle>());
    EXPECT_TRUE(vehicle.HasComponents<Physics::Component::VehicleInternal>());
    EXPECT_TRUE(vehicle.HasComponents<Physics::Component::VehicleController>());
    EXPECT_TRUE(vehicle.HasComponents<Physics::Component::RigidBody>());
    EXPECT_TRUE(vehicle.HasComponents<Object::Component::Transform>());

    // Verify vehicle internal data is valid
    const auto &vehicleInternal = vehicle.GetComponents<Physics::Component::VehicleInternal>();
    EXPECT_TRUE(vehicleInternal.IsValid());
    EXPECT_NE(vehicleInternal.vehicleConstraint, nullptr);
    EXPECT_NE(vehicleInternal.vehicleConstraint->GetController(), nullptr);
}
