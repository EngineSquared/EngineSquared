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
#include "plugin/PluginPhysics.hpp"
#include "resource/Time.hpp"
#include "utils/ShapeGenerator.hpp"

#include <cmath>
#include <glm/glm.hpp>

#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"

class TestScheduler : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};

/**
 * @brief Test wheel transform synchronization
 */
TEST(VehiclePlugin, WheelTransformSync)
{
    Engine::Core c;

    c.RegisterScheduler<TestScheduler>();
    c.SetSchedulerAfter<TestScheduler, Engine::Scheduler::Update>();
    c.SetSchedulerAfter<TestScheduler, Engine::Scheduler::FixedTimeUpdate>();
    c.SetSchedulerBefore<TestScheduler, Engine::Scheduler::Shutdown>();

    c.RegisterSystem<Engine::Scheduler::Update>(
        [&](Engine::Core &c) { c.GetResource<Engine::Resource::Time>()._elapsedTime = 1.6f; });

    c.AddPlugins<Physics::Plugin>();

    Engine::Entity vehicle = c.CreateEntity();

    c.RegisterSystem<Engine::Scheduler::Startup>([&](Engine::Core &core) {
        auto floor = Object::Helper::CreatePlane(
            core, {.width = 20.0f, .depth = 20.0f, .position = glm::vec3(0.0f, 0.0f, 0.0f)});
        floor.AddComponent<Physics::Component::BoxCollider>(
            Physics::Component::BoxCollider(glm::vec3(10.0f, 0.5f, 10.0f)));
        floor.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateStatic());

        Object::Component::Mesh chassisMesh = Object::Utils::GenerateCubeMesh(1.0f);
        Object::Component::Mesh wheelMesh = Object::Utils::GenerateWheelMesh(0.3f, 0.2f);

        Physics::Builder::VehicleBuilder<4> builder;
        vehicle = builder.SetChassisMesh(chassisMesh, glm::vec3(0.0f, 2.0f, 0.0f))
                      .SetWheelMesh(Physics::Component::WheelIndex::FrontLeft, wheelMesh)
                      .SetWheelMesh(Physics::Component::WheelIndex::FrontRight, wheelMesh)
                      .SetWheelMesh(Physics::Component::WheelIndex::RearLeft, wheelMesh)
                      .SetWheelMesh(Physics::Component::WheelIndex::RearRight, wheelMesh)
                      .Build(core);

        const auto &vehicleInternal = vehicle.GetComponents<Physics::Component::VehicleInternal>();
    });

    c.RegisterSystem<TestScheduler>([&](Engine::Core &core) {
        const auto &vehicleInternal = vehicle.GetComponents<Physics::Component::VehicleInternal>();

        for (size_t i = 0; i < 4; ++i)
        {
            auto wheelEntity = vehicleInternal.wheelEntities[i];
            ASSERT_TRUE(wheelEntity.IsValid(core));

            auto *wheelTransform = wheelEntity.TryGetComponent<Object::Component::Transform>(core);
            ASSERT_NE(wheelTransform, nullptr);

            glm::vec3 wheelPos = wheelTransform->GetPosition();
            EXPECT_TRUE(std::abs(wheelPos.x) < 100.0f && std::abs(wheelPos.y) < 100.0f && std::abs(wheelPos.z) < 100.0f)
                << "Wheel " << i << " position should be reasonable";
        }
    });
    c.RunSystems();
}
