#include <gtest/gtest.h>

#include "core/Core.hpp"
#include "component/Transform.hpp"
#include "component/Mesh.hpp"
#include "helper/CreateShape.hpp"
#include "utils/ShapeGenerator.hpp"
#include "plugin/PhysicsPlugin.hpp"
#include "component/RigidBody.hpp"
#include "component/BoxCollider.hpp"
#include "component/Vehicle.hpp"
#include "component/VehicleInternal.hpp"
#include "component/VehicleController.hpp"
#include "builder/VehicleBuilder.hpp"
#include "resource/Time.hpp"
#include "scheduler/Startup.hpp"

#include <glm/glm.hpp>

/**
 * @brief Test vehicle steering functionality
 */
TEST(VehiclePlugin, VehicleSteering)
{
    Engine::Core core;
    core.AddPlugins<Physics::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Update>(
        [&](Engine::Core &c) { c.GetResource<Engine::Resource::Time>()._elapsedTime = 0.016f; });

    core.GetScheduler<Engine::Scheduler::Startup>().RunSystems();

    auto floor = Object::Helper::CreatePlane(core, 50.0f, 50.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    auto floorCollider = Physics::Component::BoxCollider(glm::vec3(25.0f, 0.5f, 25.0f));
    floor.AddComponent<Physics::Component::BoxCollider>(core, floorCollider);
    floor.AddComponent<Physics::Component::RigidBody>(core, Physics::Component::RigidBody::CreateStatic());

    Object::Component::Mesh chassisMesh = Object::Utils::GenerateCubeMesh(1.0f);
    Object::Component::Mesh wheelMesh = Object::Utils::GenerateWheelMesh(0.3f, 0.2f);

    Physics::Builder::VehicleBuilder<4> builder;
    auto vehicle = builder.SetChassisMesh(chassisMesh, glm::vec3(0.0f, 1.0f, 0.0f))
                       .SetWheelMesh(Physics::Component::WheelIndex::FrontLeft, wheelMesh)
                       .SetWheelMesh(Physics::Component::WheelIndex::FrontRight, wheelMesh)
                       .SetWheelMesh(Physics::Component::WheelIndex::RearLeft, wheelMesh)
                       .SetWheelMesh(Physics::Component::WheelIndex::RearRight, wheelMesh)
                       .SetDrivetrain(Physics::Component::DrivetrainType::AWD)
                       .Build(core);

    auto &registry = core.GetRegistry();
    auto *controller = registry.try_get<Physics::Component::VehicleController>(vehicle);
    ASSERT_NE(controller, nullptr);

    controller->SetForward(2.0f);  // Should clamp to 1.0
    controller->SetSteering(-2.0f); // Should clamp to -1.0
    controller->SetBrake(5.0f);    // Should clamp to 1.0

    EXPECT_FLOAT_EQ(controller->forwardInput, 1.0f);
    EXPECT_FLOAT_EQ(controller->steeringInput, -1.0f);
    EXPECT_FLOAT_EQ(controller->brakeInput, 1.0f);

    controller->ResetInputs();
    EXPECT_FLOAT_EQ(controller->forwardInput, 0.0f);
    EXPECT_FLOAT_EQ(controller->steeringInput, 0.0f);
    EXPECT_FLOAT_EQ(controller->brakeInput, 0.0f);
}
