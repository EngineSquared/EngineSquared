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

#include <glm/glm.hpp>

#include "scheduler/FixedTimeUpdate.hpp"
#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"

namespace Example {
class TestScheduler : public Engine::Scheduler::Update {
  public:
    using Engine::Scheduler::Update::Update;
};

class TestSchedulerSetupPlugin : public Engine::APlugin {
  public:
    explicit TestSchedulerSetupPlugin(Engine::Core &core)
        : Engine::APlugin(core) {
              // empty
          };
    ~TestSchedulerSetupPlugin() = default;

    void Bind() final
    {
        auto &core = GetCore();
        core.RegisterScheduler<Example::TestScheduler>();
        core.SetSchedulerAfter<Example::TestScheduler, Engine::Scheduler::Update>();
        core.SetSchedulerAfter<Example::TestScheduler, Engine::Scheduler::FixedTimeUpdate>();
        core.SetSchedulerBefore<Example::TestScheduler, Engine::Scheduler::Shutdown>();
    }
};

struct DataTest {
    float startY = 0.0f;
    const float initialHeight = 5.0f;
};

class CubeFallingOnPlanePlugin : public Engine::APlugin {
  public:
    explicit CubeFallingOnPlanePlugin(Engine::Core &core) : Engine::APlugin(core) {}
    ~CubeFallingOnPlanePlugin() = default;

    void Bind() final
    {
        RequirePlugins<Physics::Plugin>();
        RequirePlugins<TestSchedulerSetupPlugin>();

        RegisterResource(DataTest{});

        RegisterSystems<Engine::Scheduler::Update>(
            [](Engine::Core &core) { core.GetResource<Engine::Resource::Time>()._elapsedTime = 3.2f; });

        RegisterSystems<Engine::Scheduler::Startup>([](Engine::Core &core) {
            auto &dataTest = core.GetResource<DataTest>();
            auto floor = Object::Helper::CreatePlane(
                core, {.width = 20.0f, .depth = 20.0f, .position = glm::vec3(0.0f, 0.0f, 0.0f)});
            auto floorCollider = Physics::Component::BoxCollider(glm::vec3(10.0f, 0.5f, 10.0f));
            floor.AddComponent<Physics::Component::BoxCollider>(floorCollider);
            floor.AddComponent<Physics::Component::RigidBody>(Physics::Component::RigidBody::CreateStatic());

            Object::Component::Mesh chassisMesh = Object::Utils::GenerateCubeMesh(1.0f);
            Object::Component::Mesh wheelMesh = Object::Utils::GenerateWheelMesh(0.3f, 0.2f);

            Physics::Builder::VehicleBuilder<4> builder;
            auto vehicle =
                core.RegisterResource(builder.SetChassisMesh(chassisMesh, glm::vec3(0.0f, dataTest.initialHeight, 0.0f))
                                          .SetWheelMesh(Physics::Component::WheelIndex::FrontLeft, wheelMesh)
                                          .SetWheelMesh(Physics::Component::WheelIndex::FrontRight, wheelMesh)
                                          .SetWheelMesh(Physics::Component::WheelIndex::RearLeft, wheelMesh)
                                          .SetWheelMesh(Physics::Component::WheelIndex::RearRight, wheelMesh)
                                          .SetChassisMass(1000.0f)
                                          .Build(core));

            auto transform = vehicle.TryGetComponent<Object::Component::Transform>();
            ASSERT_NE(transform, nullptr);

            dataTest.startY = transform->GetPosition().y;
            EXPECT_NEAR(dataTest.startY, dataTest.initialHeight, 0.1f);
        });

        RegisterSystems<TestScheduler>([](Engine::Core &core) {
            auto dataTest = core.GetResource<DataTest>();
            auto vehicle = core.GetResource<Engine::Entity>();
            auto transform = vehicle.GetComponents<Object::Component::Transform>();
            float endY = transform.GetPosition().y;
            float dropDistance = dataTest.startY - endY;

            EXPECT_GT(dropDistance, 2.0f) << "Vehicle should have fallen at least 2 meters due to gravity";
            EXPECT_LT(endY, dataTest.initialHeight) << "Vehicle Y position should be lower than initial height";
        });
    }
};

} // namespace Example

/**
 * @brief Test that a vehicle falls due to gravity
 */
TEST(VehiclePlugin, VehicleGravityDrop)
{
    Engine::Core c;

    c.AddPlugins<Example::CubeFallingOnPlanePlugin>();

    c.RunSystems();
}
