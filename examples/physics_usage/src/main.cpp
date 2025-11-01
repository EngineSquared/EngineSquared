/**************************************************************************
 * EngineSquared - Physics Usage Example
 *
 * This example demonstrates how to use the Physics plugin with RigidBody
 * components to create a simple physics simulation with falling cubes.
 **************************************************************************/

#include "Engine.hpp"
#include "Object.hpp"
#include "Physics.hpp"

#include "resource/Time.hpp"

#include <iostream>

/**
 * @brief Create a static floor plane using Object::CreatePlane helper
 */
void CreateFloor(Engine::Core &core)
{
    // Use the helper function to create a plane entity with mesh and transform
    auto floor = Object::Resource::CreatePlane(core, 20.0f, 20.0f, glm::vec3(0.0f, 0.0f, 0.0f));

    // Large flat box collider (20x1x20 meters)
    auto boxCollider = Physics::Component::BoxCollider(glm::vec3(10.0f, 0.5f, 10.0f));
    floor.AddComponent<Physics::Component::BoxCollider>(core, boxCollider);

    // Static rigid body (immovable)
    floor.AddComponent<Physics::Component::RigidBody>(core, Physics::Component::RigidBody::CreateStatic());

    std::cout << "- Floor created (static body with mesh)" << std::endl;
}

/**
 * @brief Create a dynamic cube that will fall using Object::CreateCube helper
 */
void CreateFallingCube(Engine::Core &core, float x, float y, float z, float mass)
{
    // Use the helper function to create a cube entity with mesh and transform
    auto cube = Object::Resource::CreateCube(core, 1.0f, glm::vec3(x, y, z));

    // Physics properties
    auto rigidBody = Physics::Component::RigidBody::CreateDynamic(mass);
    rigidBody.friction = 0.5f;
    rigidBody.restitution = 0.3f; // Some bounce
    cube.AddComponent<Physics::Component::RigidBody>(core, rigidBody);

    // DefaultCollider is created automatically!

    std::cout << "- Cube created at (" << x << ", " << y << ", " << z << ") with mass " << mass << "kg (with mesh)"
              << std::endl;
}

/**
 * @brief Create a bouncy ball with high restitution using Object::CreateSphere helper
 */
void CreateBouncyBall(Engine::Core &core, float x, float y, float z)
{
    // Use the helper function to create a sphere entity with mesh and transform
    auto ball = Object::Resource::CreateSphere(core, 0.5f, glm::vec3(x, y, z));

    // Sphere-like collider (using small cube for now as approximation)
    auto collider = Physics::Component::BoxCollider(glm::vec3(0.5f, 0.5f, 0.5f));
    ball.AddComponent<Physics::Component::BoxCollider>(core, collider);

    // Very bouncy properties
    auto rigidBody = Physics::Component::RigidBody::CreateDynamic(1.0f);
    rigidBody.restitution = 0.8f; // 80% bounce
    rigidBody.friction = 0.2f;    // Low friction
    ball.AddComponent<Physics::Component::RigidBody>(core, rigidBody);

    std::cout << "- Bouncy ball created at (" << x << ", " << y << ", " << z << ") with mesh" << std::endl;
}

/**
 * @brief Create a kinematic platform that can be moved programmatically
 */
void CreateMovingPlatform(Engine::Core &core)
{
    // Use the helper function to create a cube entity (flattened for platform shape)
    auto platform =
        Object::Resource::CreateCube(core, 1.0f, glm::vec3(0.0f, 5.0f, 0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                                     glm::vec3(3.0f, 0.3f, 3.0f)); // Scale to make it flat

    // Platform collider
    auto collider = Physics::Component::BoxCollider(glm::vec3(3.0f, 0.3f, 3.0f));
    platform.AddComponent<Physics::Component::BoxCollider>(core, collider);

    // Kinematic body (can be moved programmatically)
    platform.AddComponent<Physics::Component::RigidBody>(core, Physics::Component::RigidBody::CreateKinematic());

    std::cout << "- Kinematic platform created with mesh" << std::endl;
}

/**
 * @brief Display simulation statistics
 */
void PrintSimulationStep(int step, Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    // Count active bodies
    int dynamicBodies = 0;
    int staticBodies = 0;
    int kinematicBodies = 0;

    registry.view<Physics::Component::RigidBody>().each([&](const Physics::Component::RigidBody &rb) {
        switch (rb.motionType)
        {
        case Physics::Component::MotionType::Dynamic: dynamicBodies++; break;
        case Physics::Component::MotionType::Static: staticBodies++; break;
        case Physics::Component::MotionType::Kinematic: kinematicBodies++; break;
        }
    });

    std::cout << "\n[Step " << step << "] "
              << "Dynamic: " << dynamicBodies << " | "
              << "Static: " << staticBodies << " | "
              << "Kinematic: " << kinematicBodies << std::endl;
}

struct CounterComponent {
    uint32_t counter;
    const uint32_t NUM_STEPS = 100u;

    CounterComponent(uint32_t initialCount) : counter(initialCount) {}
};

void RunningPhysicsSimulationSystem(Engine::Core &core)
{
    core.GetRegistry().view<CounterComponent>().each([&](CounterComponent &counter) {
        if (counter.counter >= counter.NUM_STEPS)
        {
            core.Stop();
            return;
        }

        counter.counter++;

        // Print info every 10 steps
        if (counter.counter % 10 != 0)
            return;

        PrintSimulationStep(counter.counter, core);

        // Sample: Show positions of all dynamic cubes
        std::cout << "  - Dynamic body positions:" << std::endl;
        core.GetRegistry().view<Physics::Component::RigidBody, Object::Component::Transform>().each(
            [&](Engine::Entity entity, const Physics::Component::RigidBody &rb,
                const Object::Component::Transform &transform) {
                if (rb.motionType != Physics::Component::MotionType::Dynamic)
                    return;

                std::cout << "    Entity " << static_cast<uint32_t>(entity) << ": (" << transform.position.x << ", "
                          << transform.position.y << ", " << transform.position.z << ")" << std::endl;
            });
    });
}

void CreatePhysicsWorldSystem(Engine::Core &core)
{
    std::cout << "Creating physics scene..." << std::endl;

    // 1. Static floor
    CreateFloor(core);

    // 2. Several falling cubes with different properties
    CreateFallingCube(core, 0.0f, 10.0f, 0.0f, 5.0f);   // Center, 5kg
    CreateFallingCube(core, 2.0f, 15.0f, 2.0f, 10.0f);  // Right, 10kg
    CreateFallingCube(core, -2.0f, 12.0f, -2.0f, 2.0f); // Left, 2kg

    // 3. Bouncy ball
    CreateBouncyBall(core, 3.0f, 20.0f, 0.0f);

    // 4. Kinematic platform
    CreateMovingPlatform(core);

    std::cout << "\n✓ Scene setup complete!\n" << std::endl;

    auto counter = core.CreateEntity();
    counter.AddComponent<CounterComponent>(core, 0u);
}

void CleanupDemonstrationSystem(Engine::Core &core)
{
    std::cout << "\nCleaning up entities..." << std::endl;
    int removedCount = 0;
    core.GetRegistry().view<Physics::Component::RigidBody>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
            core.KillEntity(entity);
            removedCount++;
        });
    core.GetRegistry().view<CounterComponent>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const CounterComponent &cc) {
            core.KillEntity(entity);
            removedCount++;
        });
    std::cout << "- Removed " << removedCount << " entities" << std::endl;
}

int main(void)
{
    std::cout << "========================================" << std::endl;
    std::cout << "  EngineSquared - Physics Example" << std::endl;
    std::cout << "========================================\n" << std::endl;

    Engine::Core core;

    std::cout << "Registering plugins..." << std::endl;
    core.AddPlugins<Physics::Plugin>();
    std::cout << "- Plugins registered\n" << std::endl;

    core.RegisterSystem<Engine::Scheduler::Startup>(CreatePhysicsWorldSystem);
    core.RegisterSystem<Engine::Scheduler::FixedTimeUpdate>(RunningPhysicsSimulationSystem);
    core.RegisterSystem<Engine::Scheduler::Shutdown>(CleanupDemonstrationSystem);

    std::cout << "========================================" << std::endl;
    std::cout << "  Running Physics Simulation" << std::endl;
    std::cout << "========================================" << std::endl;

    core.RunCore();

    std::cout << "\n========================================" << std::endl;
    std::cout << "  Simulation Complete!" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
