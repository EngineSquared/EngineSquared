#include <gtest/gtest.h>

#include "builder/VehicleBuilder.hpp"
#include "component/BoxCollider.hpp"
#include "component/Mesh.hpp"
#include "component/RigidBody.hpp"
#include "component/Transform.hpp"
#include "component/Vehicle.hpp"
#include "component/VehicleController.hpp"
#include "component/VehicleInternal.hpp"
#include "core/Core.hpp"
#include "helper/CreateShape.hpp"
#include "plugin/PhysicsPlugin.hpp"
#include "resource/Time.hpp"
#include "utils/ShapeGenerator.hpp"

#include <glm/glm.hpp>

/**
 * @brief Test that a vehicle falls due to gravity
 */
TEST(VehiclePlugin, VehicleGravityDrop)
{
    Engine::Core core;

    core.RegisterSystem<Engine::Scheduler::Update>(
        [&](Engine::Core &c) { c.GetResource<Engine::Resource::Time>()._elapsedTime = 0.016f; });

    core.AddPlugins<Physics::Plugin>();
    core.RunSystems();

    auto floor = Object::Helper::CreatePlane(core, 20.0f, 20.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    auto floorCollider = Physics::Component::BoxCollider(glm::vec3(10.0f, 0.5f, 10.0f));
    floor.AddComponent<Physics::Component::BoxCollider>(floorCollider);
    floor.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateStatic());

    Object::Component::Mesh chassisMesh = Object::Utils::GenerateCubeMesh(1.0f);
    Object::Component::Mesh wheelMesh = Object::Utils::GenerateWheelMesh(0.3f, 0.2f);

    float initialHeight = 5.0f;
    Physics::Builder::VehicleBuilder<4> builder;
    auto vehicle = builder.SetChassisMesh(chassisMesh, glm::vec3(0.0f, initialHeight, 0.0f))
                       .SetWheelMesh(Physics::Component::WheelIndex::FrontLeft, wheelMesh)
                       .SetWheelMesh(Physics::Component::WheelIndex::FrontRight, wheelMesh)
                       .SetWheelMesh(Physics::Component::WheelIndex::RearLeft, wheelMesh)
                       .SetWheelMesh(Physics::Component::WheelIndex::RearRight, wheelMesh)
                       .SetChassisMass(1000.0f)
                       .Build(core);

    auto &registry = core.GetRegistry();
    auto *transform = vehicle.TryGetComponent<Object::Component::Transform>();
    ASSERT_NE(transform, nullptr);

    float startY = transform->GetPosition().y;
    EXPECT_NEAR(startY, initialHeight, 0.1f);

    for (int i = 0; i < 200; ++i)
    {
        core.RunSystems();
    }

    float endY = transform->GetPosition().y;
    float dropDistance = startY - endY;

    EXPECT_GT(dropDistance, 2.0f) << "Vehicle should have fallen at least 2 meters due to gravity";
    EXPECT_LT(endY, initialHeight) << "Vehicle Y position should be lower than initial height";
}
