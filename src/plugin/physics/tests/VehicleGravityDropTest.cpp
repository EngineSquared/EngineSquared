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

#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"

class TestScheduler : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};

/**
 * @brief Test that a vehicle falls due to gravity
 */
TEST(VehiclePlugin, VehicleGravityDrop)
{
    Engine::Core c;

    c.RegisterScheduler<TestScheduler>();
    c.SetSchedulerAfter<TestScheduler, Engine::Scheduler::Update>();
    c.SetSchedulerAfter<TestScheduler, Engine::Scheduler::FixedTimeUpdate>();
    c.SetSchedulerBefore<TestScheduler, Engine::Scheduler::Shutdown>();

    c.RegisterSystem<Engine::Scheduler::Update>(
        [&](Engine::Core &c) { c.GetResource<Engine::Resource::Time>()._elapsedTime = 3.2f; });

    c.AddPlugins<Physics::Plugin>();

    Engine::Entity vehicle = c.CreateEntity();

    float startY = 0.0f;
    float initialHeight = 5.0f;

    c.RegisterSystem<Engine::Scheduler::Startup>([&](Engine::Core &core) {
        auto floor = Object::Helper::CreatePlane(
            core, {.width = 20.0f, .depth = 20.0f, .position = glm::vec3(0.0f, 0.0f, 0.0f)});
        auto floorCollider = Physics::Component::BoxCollider(glm::vec3(10.0f, 0.5f, 10.0f));
        floor.AddComponent<Physics::Component::BoxCollider>(floorCollider);
        floor.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateStatic());

        Object::Component::Mesh chassisMesh = Object::Utils::GenerateCubeMesh(1.0f);
        Object::Component::Mesh wheelMesh = Object::Utils::GenerateWheelMesh(0.3f, 0.2f);

        Physics::Builder::VehicleBuilder<4> builder;
        vehicle = builder.SetChassisMesh(chassisMesh, glm::vec3(0.0f, initialHeight, 0.0f))
                      .SetWheelMesh(Physics::Component::WheelIndex::FrontLeft, wheelMesh)
                      .SetWheelMesh(Physics::Component::WheelIndex::FrontRight, wheelMesh)
                      .SetWheelMesh(Physics::Component::WheelIndex::RearLeft, wheelMesh)
                      .SetWheelMesh(Physics::Component::WheelIndex::RearRight, wheelMesh)
                      .SetChassisMass(1000.0f)
                      .Build(core);

        auto transform = vehicle.TryGetComponent<Object::Component::Transform>();
        ASSERT_NE(transform, nullptr);

        startY = transform->GetPosition().y;
        EXPECT_NEAR(startY, initialHeight, 0.1f);
    });

    c.RegisterSystem<TestScheduler>([&](Engine::Core &core) {
        auto transform = vehicle.TryGetComponent<Object::Component::Transform>();
        float endY = transform->GetPosition().y;
        float dropDistance = startY - endY;

        EXPECT_GT(dropDistance, 2.0f) << "Vehicle should have fallen at least 2 meters due to gravity";
        EXPECT_LT(endY, initialHeight) << "Vehicle Y position should be lower than initial height";
    });

    c.RunSystems();
}
