#include "FallingObjectsExample.hpp"
#include "Object.hpp"

#include <iostream>

namespace Examples {

// ============================================================================
// Component for simulation counter
// ============================================================================

struct SimulationCounterComponent {
    uint32_t currentStep = 0u;
    const uint32_t MAX_STEPS = 100u;

    explicit SimulationCounterComponent() = default;
};

// ============================================================================
// Entity Creation Functions
// ============================================================================

/**
 * @brief Create a static floor plane using Object::CreatePlane helper
 *
 * Use the helper function to create a plane entity with mesh and transform.
 * Add Large flat box collider (20x1x20 meters) component for collision and
 * Static rigid body (immovable) component.
 */
static void CreateFloor(Engine::Core &core)
{
    auto floor = Object::Helper::CreatePlane(core, 20.0f, 20.0f, glm::vec3(0.0f, 0.0f, 0.0f));

    auto boxCollider = Physics::Component::BoxCollider(glm::vec3(10.0f, 0.5f, 10.0f));
    floor.AddComponent<Physics::Component::BoxCollider>(core, boxCollider);

    floor.AddComponent<Physics::Component::RigidBody>(core, Physics::Component::RigidBody::CreateStatic());

    std::cout << "  - Floor created (static body)" << std::endl;
}

/**
 * @brief Create a dynamic cube that will fall using Object::CreateCube helper
 *
 * Use the helper function to create a cube entity with mesh and transform.
 * Add RigidBody component with dynamic motion type and specified mass.
 * The DefaultCollider is created automatically!
 */
static void CreateFallingCube(Engine::Core &core, float x, float y, float z, float mass)
{
    auto cube = Object::Helper::CreateCube(core, 1.0f, glm::vec3(x, y, z));

    auto rigidBody = Physics::Component::RigidBody::CreateDynamic(mass);
    rigidBody.friction = 0.5f;
    rigidBody.restitution = 0.3f; // Some bounce
    cube.AddComponent<Physics::Component::RigidBody>(core, rigidBody);

    std::cout << "  - Cube created at (" << x << ", " << y << ", " << z << ") with mass " << mass << "kg" << std::endl;
}

/**
 * @brief Create a bouncy ball with high restitution using Object::CreateSphere helper
 *
 * Use the helper function to create a sphere entity with mesh and transform.
 * Add Sphere-like collider (using small cube for now as approximation),
 * then Very bouncy properties
 */
static void CreateBouncyBall(Engine::Core &core, float x, float y, float z)
{
    auto ball = Object::Helper::CreateSphere(core, 0.5f, glm::vec3(x, y, z));

    auto collider = Physics::Component::BoxCollider(glm::vec3(0.5f, 0.5f, 0.5f));
    ball.AddComponent<Physics::Component::BoxCollider>(core, collider);

    auto rigidBody = Physics::Component::RigidBody::CreateDynamic(1.0f);
    rigidBody.restitution = 0.8f; // 80% bounce
    rigidBody.friction = 0.2f;    // Low friction
    ball.AddComponent<Physics::Component::RigidBody>(core, rigidBody);

    std::cout << "  - Bouncy ball created at (" << x << ", " << y << ", " << z << ")" << std::endl;
}

/**
 * @brief Create a kinematic platform that can be moved programmatically
 *
 * Use the helper function to create a cube entity (flattened for platform shape)
 */
static void CreateMovingPlatform(Engine::Core &core)
{
    auto platform = Object::Helper::CreateCube(core, 1.0f, glm::vec3(0.0f, 5.0f, 0.0f),
                                               glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3(3.0f, 0.3f, 3.0f));

    auto collider = Physics::Component::BoxCollider(glm::vec3(3.0f, 0.3f, 3.0f));
    platform.AddComponent<Physics::Component::BoxCollider>(core, collider);

    platform.AddComponent<Physics::Component::RigidBody>(core, Physics::Component::RigidBody::CreateKinematic());

    std::cout << "  - Kinematic platform created" << std::endl;
}

void OnCollisionAdded(Engine::Core &core, const Physics::Event::CollisionAddedEvent &event)
{
    std::cout << "Collision detected between Entity " << static_cast<uint32_t>(event.entity1) << " and Entity "
              << static_cast<uint32_t>(event.entity2) << std::endl;
}

void RegisterCollisionLoggerSystem(Engine::Core &core)
{
    auto &eventManager = core.GetResource<Event::Resource::EventManager>();
    eventManager.RegisterCallback<Physics::Event::CollisionAddedEvent>(OnCollisionAdded);
}

// ============================================================================
// Systems
// ============================================================================

/**
 * @brief Create the physics world with several entities
 *
 * 1. Static floor
 * 2. Several falling cubes with different properties
 * 3. Bouncy ball
 * 4. Kinematic platform
 */
static void SetupSceneSystem(Engine::Core &core)
{
    std::cout << "\n[Falling Objects Example]" << std::endl;
    std::cout << "Creating physics scene..." << std::endl;

    CreateFloor(core);
    CreateFallingCube(core, 0.0f, 10.0f, 0.0f, 5.0f);   // Center, 5kg
    CreateFallingCube(core, 2.0f, 15.0f, 2.0f, 10.0f);  // Right, 10kg
    CreateFallingCube(core, -2.0f, 12.0f, -2.0f, 2.0f); // Left, 2kg
    CreateBouncyBall(core, 3.0f, 20.0f, 0.0f);
    CreateMovingPlatform(core);

    std::cout << "✓ Scene setup complete!" << std::endl;

    // Create counter entity
    auto counter = core.CreateEntity();
    counter.AddComponent<SimulationCounterComponent>(core);
}

/**
 * @brief Runs the physics simulation system and prints info every 10 steps
 */
static void SimulationUpdateSystem(Engine::Core &core)
{
    core.GetRegistry().view<SimulationCounterComponent>().each([&](SimulationCounterComponent &counter) {
        if (counter.currentStep >= counter.MAX_STEPS)
        {
            core.Stop();
            return;
        }

        counter.currentStep++;

        // Print stats every 10 steps
        if (counter.currentStep % 10 != 0)
            return;

        auto &registry = core.GetRegistry();
        uint32_t dynamicBodies = 0;
        uint32_t staticBodies = 0;
        uint32_t kinematicBodies = 0;

        registry.view<Physics::Component::RigidBody>().each(
            [&]([[maybe_unused]] Engine::Entity entity, const Physics::Component::RigidBody &rb) {
                switch (rb.motionType)
                {
                case Physics::Component::MotionType::Dynamic: dynamicBodies++; break;
                case Physics::Component::MotionType::Static: staticBodies++; break;
                case Physics::Component::MotionType::Kinematic: kinematicBodies++; break;
                }
            });

        std::cout << "\n[Step " << counter.currentStep << "] " << "Dynamic: " << dynamicBodies << " | "
                  << "Static: " << staticBodies << " | " << "Kinematic: " << kinematicBodies << std::endl;

        // Print positions of dynamic bodies
        std::cout << "  Dynamic body positions:" << std::endl;
        registry.view<Physics::Component::RigidBody, Object::Component::Transform>().each(
            [&](Engine::Entity entity, const Physics::Component::RigidBody &rb,
                const Object::Component::Transform &transform) {
                if (rb.motionType != Physics::Component::MotionType::Dynamic)
                    return;

                std::cout << "    Entity " << static_cast<uint32_t>(entity) << ": (" << transform.position.x << ", "
                          << transform.position.y << ", " << transform.position.z << ")" << std::endl;
            });
    });
}

/**
 * @brief Cleanup all created entities in the physics demonstration
 */
static void CleanupSystem(Engine::Core &core)
{
    std::cout << "\nCleaning up Falling Objects example..." << std::endl;

    int removedCount = 0;

    core.GetRegistry().view<Physics::Component::RigidBody>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
            core.KillEntity(entity);
            removedCount++;
        });

    core.GetRegistry().view<SimulationCounterComponent>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const SimulationCounterComponent &sc) {
            core.KillEntity(entity);
            removedCount++;
        });

    std::cout << "  - Removed " << removedCount << " entities" << std::endl;
}

// ============================================================================
// Plugin Registration
// ============================================================================

void FallingObjectsExample::Bind()
{
    RequirePlugins<Physics::Plugin>();
    RegisterSystems<Engine::Scheduler::Startup>(SetupSceneSystem, RegisterCollisionLoggerSystem);
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(SimulationUpdateSystem);
    RegisterSystems<Engine::Scheduler::Shutdown>(CleanupSystem);
}

} // namespace Examples
