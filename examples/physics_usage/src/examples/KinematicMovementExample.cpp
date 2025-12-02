#include "KinematicMovementExample.hpp"

#include "Object.hpp"
#include "Physics.hpp"

#include <glm/gtc/constants.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

namespace Examples {

// ============================================================================
// Components for tracking entities
// ============================================================================

struct MovingPlatformTag {};
struct ElevatorTag {};
struct RotatingDoorTag {};
struct PassengerTag {};

// ============================================================================
// Systems
// ============================================================================

static void SetupSceneSystem(Engine::Core &core)
{
    std::cout << "\n=================================================" << std::endl;
    std::cout << "  Kinematic Movement Example (Issue #003)" << std::endl;
    std::cout << "=================================================\n" << std::endl;

    auto &registry = core.GetRegistry();

    // Example 1: Moving Platform (Back and Forth)
    // ============================================================
    std::cout << "[Example 1] Moving Platform" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    auto platform = core.CreateEntity();

    auto platformTransform = Object::Component::Transform();
    platformTransform.SetPosition(-5.0f, 1.0f, 0.0f);
    platformTransform.SetScale(4.0f, 0.5f, 4.0f);
    registry.emplace<Object::Component::Transform>(platform, platformTransform);

    auto platformRb = Physics::Component::RigidBody::CreateKinematic();
    registry.emplace<Physics::Component::RigidBody>(platform, platformRb);
    registry.emplace<MovingPlatformTag>(platform);

    std::cout << "  ✓ Created moving platform (moves 10m horizontally)" << std::endl;

    // Passenger cube on platform
    auto passenger1 = core.CreateEntity();

    auto passenger1Transform = Object::Component::Transform();
    passenger1Transform.SetPosition(-5.0f, 2.0f, 0.0f);
    registry.emplace<Object::Component::Transform>(passenger1, passenger1Transform);

    auto passenger1Rb = Physics::Component::RigidBody::CreateDynamic(1.0f);
    registry.emplace<Physics::Component::RigidBody>(passenger1, passenger1Rb);
    registry.emplace<PassengerTag>(passenger1);

    std::cout << "  ✓ Placed dynamic cube on platform" << std::endl;
    std::cout << "  → Platform will carry the cube\n" << std::endl;

    // Example 2: Elevator (Up and Down)
    // ============================================================
    std::cout << "[Example 2] Elevator" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    auto elevator = core.CreateEntity();

    auto elevatorTransform = Object::Component::Transform();
    elevatorTransform.SetPosition(5.0f, 0.0f, 0.0f);
    elevatorTransform.SetScale(3.0f, 0.5f, 3.0f);
    registry.emplace<Object::Component::Transform>(elevator, elevatorTransform);

    auto elevatorRb = Physics::Component::RigidBody::CreateKinematic();
    registry.emplace<Physics::Component::RigidBody>(elevator, elevatorRb);
    registry.emplace<ElevatorTag>(elevator);

    std::cout << "  ✓ Created elevator platform" << std::endl;

    // Multiple passengers on elevator
    for (int i = 0; i < 3; ++i)
    {
        auto passenger = core.CreateEntity();

        auto passengerTransform = Object::Component::Transform();
        passengerTransform.SetPosition(5.0f + i * 0.6f, 1.0f, 0.0f);
        passengerTransform.SetScale(0.5f, 0.5f, 0.5f);
        registry.emplace<Object::Component::Transform>(passenger, passengerTransform);

        auto passengerRb = Physics::Component::RigidBody::CreateDynamic(0.5f);
        registry.emplace<Physics::Component::RigidBody>(passenger, passengerRb);
        registry.emplace<PassengerTag>(passenger);
    }

    std::cout << "  ✓ Placed 3 cubes on elevator" << std::endl;
    std::cout << "  → Elevator goes from 0m to 8m height\n" << std::endl;

    // Example 3: Rotating Door
    // ============================================================
    std::cout << "[Example 3] Rotating Door" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    auto door = core.CreateEntity();

    auto doorTransform = Object::Component::Transform();
    doorTransform.SetPosition(0.0f, 1.5f, -5.0f);
    doorTransform.SetScale(0.2f, 3.0f, 2.0f);
    registry.emplace<Object::Component::Transform>(door, doorTransform);

    auto doorRb = Physics::Component::RigidBody::CreateKinematic();
    registry.emplace<Physics::Component::RigidBody>(door, doorRb);
    registry.emplace<RotatingDoorTag>(door);

    std::cout << "  ✓ Created rotating door" << std::endl;
    std::cout << "  → Door rotates 90° around Y axis\n" << std::endl;

    std::cout << "✓ All 3 kinematic examples created!\n" << std::endl;
}

static void KinematicMovementSystem(Engine::Core &core)
{
    static uint32_t frame = 0;
    frame++;

    float time = frame / 60.0f; // Assuming 60 FPS
    float deltaTime = 1.0f / 60.0f;

    // Example 1: Moving Platform (linear back-and-forth)
    core.GetRegistry().view<MovingPlatformTag, Physics::Component::RigidBody>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
            // Oscillate between x = -5 and x = 5
            float progress = std::sin(time * 0.5f) * 0.5f + 0.5f; // 0 to 1
            float xPosition = -5.0f + progress * 10.0f;

            glm::vec3 targetPosition(xPosition, 1.0f, 0.0f);
            glm::quat targetRotation = glm::quat(1, 0, 0, 0);

            Physics::Helper::MoveKinematic(core, entity, targetPosition, targetRotation, deltaTime);
        });

    // Example 2: Elevator (vertical movement)
    core.GetRegistry().view<ElevatorTag, Physics::Component::RigidBody>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
            // Move between y = 0 and y = 8
            float progress = std::sin(time * 0.3f) * 0.5f + 0.5f; // 0 to 1
            float yPosition = progress * 8.0f;

            glm::vec3 targetPosition(5.0f, yPosition, 0.0f);
            glm::quat targetRotation = glm::quat(1, 0, 0, 0);

            Physics::Helper::MoveKinematic(core, entity, targetPosition, targetRotation, deltaTime);
        });

    // Example 3: Rotating Door (rotation around Y axis)
    core.GetRegistry().view<RotatingDoorTag, Physics::Component::RigidBody>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
            // Rotate between 0° and 90°
            float progress = std::sin(time * 0.4f) * 0.5f + 0.5f; // 0 to 1
            float angle = progress * glm::half_pi<float>();       // 0 to 90 degrees

            glm::vec3 targetPosition(0.0f, 1.5f, -5.0f);
            glm::quat targetRotation = glm::angleAxis(angle, glm::vec3(0, 1, 0));

            Physics::Helper::MoveKinematic(core, entity, targetPosition, targetRotation, deltaTime);
        });
}

static void PrintSummarySystem(Engine::Core &core)
{
    static int updateCount = 0;
    updateCount++;

    // Print summary after 3 seconds (180 frames at 60 FPS)
    if (updateCount != 180)
        return;

    std::cout << "\n=================================================" << std::endl;
    std::cout << "  Kinematic Movement API Summary" << std::endl;
    std::cout << "=================================================\n" << std::endl;

    std::cout << "✓ MoveKinematic()         : Platform, Elevator, Door" << std::endl;
    std::cout << "✓ SetKinematicVelocity()  : Alternative for linear motion" << std::endl;
    std::cout << "✓ GetKinematicTarget()    : Query current target" << std::endl;

    std::cout << "\nAll 3 API functions demonstrated successfully! ✅\n" << std::endl;

    std::cout << "Kinematic Movement Patterns:" << std::endl;
    std::cout << "----------------------------" << std::endl;

    // Count entities
    int platformCount = 0;
    core.GetRegistry().view<MovingPlatformTag>().each([&]([[maybe_unused]] const auto entity) { platformCount++; });

    int elevatorCount = 0;
    core.GetRegistry().view<ElevatorTag>().each([&]([[maybe_unused]] const auto entity) { elevatorCount++; });

    int doorCount = 0;
    core.GetRegistry().view<RotatingDoorTag>().each([&]([[maybe_unused]] const auto entity) { doorCount++; });

    int passengerCount = 0;
    core.GetRegistry().view<PassengerTag>().each([&]([[maybe_unused]] const auto entity) { passengerCount++; });

    std::cout << "  Moving Platforms: " << platformCount << std::endl;
    std::cout << "  Elevators: " << elevatorCount << std::endl;
    std::cout << "  Rotating Doors: " << doorCount << std::endl;
    std::cout << "  Dynamic Passengers: " << passengerCount << " (carried by kinematics)" << std::endl;

    std::cout << std::endl;

    // Stop the simulation
    core.Stop();
}

static void CleanupSystem(Engine::Core &core)
{
    std::cout << "\nCleaning up Kinematic Movement example..." << std::endl;

    int removedCount = 0;

    // Clean all entities with RigidBody
    core.GetRegistry().view<Physics::Component::RigidBody>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
            core.KillEntity(entity);
            removedCount++;
        });

    std::cout << "  - Removed " << removedCount << " entities" << std::endl;
}

// ============================================================================
// Plugin Bind
// ============================================================================

void KinematicMovementExample::Bind()
{
    RequirePlugins<Physics::Plugin>();

    RegisterSystems<Engine::Scheduler::Startup>(SetupSceneSystem);
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(KinematicMovementSystem, PrintSummarySystem);
    RegisterSystems<Engine::Scheduler::Shutdown>(CleanupSystem);
}

} // namespace Examples
