#include "ForcesImpulsesExample.hpp"

#include <iostream>
#include <vector>

namespace Examples {

// ============================================================================
// Components for tracking entities
// ============================================================================

struct PropelledCubeTag {};
struct SpinnerTag {};
struct DebrisTag {
    glm::vec3 position;
};

// ============================================================================
// Systems
// ============================================================================

static void SetupSceneSystem(Engine::Core &core)
{
    std::cout << "\n=================================================" << std::endl;
    std::cout << "  Forces & Impulses Example (Issue #001)" << std::endl;
    std::cout << "=================================================\n" << std::endl;

    // Example 1: Propelled Cube with continuous force
    std::cout << "[Example 1] Propelled Cube - Continuous Upward Force" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    auto propelledCube = Engine::Entity::Create(core);
    auto &rb1 = propelledCube.AddComponent<Physics::Component::RigidBody>(core);
    rb1.motionType = Physics::Component::MotionType::Dynamic;
    rb1.mass = 2.0f;
    rb1.linearDamping = 0.1f;
    propelledCube.AddComponent<PropelledCubeTag>(core);

    std::cout << "  ✓ Created dynamic cube (mass = 2 kg)" << std::endl;
    std::cout << "  → Will apply 20N upward force each frame\n" << std::endl;

    // Example 2: Explosion with radial impulses
    std::cout << "[Example 2] Explosion - Radial Impulses" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    glm::vec3 explosionCenter(0.0f, 5.0f, 0.0f);
    std::vector<glm::vec3> positions = {
        {-3.0f, 5.0f, 0.0f },
        {3.0f,  5.0f, 0.0f },
        {0.0f,  5.0f, -3.0f},
        {0.0f,  5.0f, 3.0f }
    };

    for (const auto &pos : positions)
    {
        auto debris = Engine::Entity::Create(core);
        auto &rb = debris.AddComponent<Physics::Component::RigidBody>(core);
        rb.motionType = Physics::Component::MotionType::Dynamic;
        rb.mass = 1.0f;
        rb.restitution = 0.3f;

        DebrisTag tag;
        tag.position = pos;
        debris.AddComponent<DebrisTag>(core, tag);
    }

    std::cout << "  ✓ Created 4 debris cubes" << std::endl;
    std::cout << "  → Will apply explosive impulses from center\n" << std::endl;

    // Example 3: Spinner with continuous torque
    std::cout << "[Example 3] Spinner - Continuous Torque" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    auto spinner = Engine::Entity::Create(core);
    auto &rb3 = spinner.AddComponent<Physics::Component::RigidBody>(core);
    rb3.motionType = Physics::Component::MotionType::Dynamic;
    rb3.mass = 3.0f;
    rb3.angularDamping = 0.05f;
    spinner.AddComponent<SpinnerTag>(core);

    std::cout << "  ✓ Created spinner object (mass = 3 kg)" << std::endl;
    std::cout << "  → Will apply 10 N·m torque around Y-axis\n" << std::endl;

    // Example 4: Door with force at point
    std::cout << "[Example 4] Door - Force at Point" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    auto door = Engine::Entity::Create(core);
    auto &rb4 = door.AddComponent<Physics::Component::RigidBody>(core);
    rb4.motionType = Physics::Component::MotionType::Dynamic;
    rb4.mass = 5.0f;
    rb4.friction = 0.8f;
    rb4.angularDamping = 0.2f;

    glm::vec3 doorCenter(0.0f, 1.5f, 0.0f);
    glm::vec3 pushPoint = doorCenter + glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 pushForce(0.0f, 0.0f, 15.0f);

    Physics::Helper::AddForceAtPoint(core, door, pushForce, pushPoint);

    std::cout << "  ✓ Created door (mass = 5 kg)" << std::endl;
    std::cout << "  ✓ Applied 15N force at edge (creates rotation)\n" << std::endl;

    // Example 5: Flywheel with angular impulse
    std::cout << "[Example 5] Flywheel - Angular Impulse" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    auto flywheel = Engine::Entity::Create(core);
    auto &rb5 = flywheel.AddComponent<Physics::Component::RigidBody>(core);
    rb5.motionType = Physics::Component::MotionType::Dynamic;
    rb5.mass = 10.0f;
    rb5.angularDamping = 0.01f;

    glm::vec3 angularImpulse(0.0f, 20.0f, 0.0f);
    Physics::Helper::AddAngularImpulse(core, flywheel, angularImpulse);

    std::cout << "  ✓ Created flywheel (mass = 10 kg)" << std::endl;
    std::cout << "  ✓ Applied 20 N·m·s angular impulse (instant spin)\n" << std::endl;

    // Example 6: Baseball with impulse at point
    std::cout << "[Example 6] Baseball Bat Hit - Impulse at Point" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    auto ball = Engine::Entity::Create(core);
    auto &rb6 = ball.AddComponent<Physics::Component::RigidBody>(core);
    rb6.motionType = Physics::Component::MotionType::Dynamic;
    rb6.mass = 0.145f;
    rb6.restitution = 0.5f;

    glm::vec3 ballCenter(0.0f, 1.0f, 0.0f);
    glm::vec3 hitPoint = ballCenter + glm::vec3(0.0f, 0.05f, 0.0f);
    glm::vec3 batImpulse(20.0f, 5.0f, 0.0f);

    Physics::Helper::AddImpulseAtPoint(core, ball, batImpulse, hitPoint);

    std::cout << "  ✓ Created baseball (mass = 145 g)" << std::endl;
    std::cout << "  ✓ Applied impulse at point (backspin launch)\n" << std::endl;

    std::cout << "\n✓ All 6 examples created and initialized!" << std::endl;
}

static void ApplyContinuousForcesSystem(Engine::Core &core)
{
    static int frame = 0;
    frame++;

    // Only apply forces for first 60 frames (1 second at 60 FPS)
    if (frame > 60)
        return;

    // Example 1: Apply continuous force to propelled cube
    core.GetRegistry().view<PropelledCubeTag, Physics::Component::RigidBody>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
            glm::vec3 thrustForce(0.0f, 20.0f, 0.0f);
            Physics::Helper::AddForce(core, entity, thrustForce);
        });

    // Example 3: Apply continuous torque to spinner
    core.GetRegistry().view<SpinnerTag, Physics::Component::RigidBody>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
            glm::vec3 torque(0.0f, 10.0f, 0.0f);
            Physics::Helper::AddTorque(core, entity, torque);
        });

    if (frame == 1)
    {
        std::cout << "\n[Physics Update] Applying continuous forces..." << std::endl;
    }
}

static void ApplyExplosionImpulsesSystem(Engine::Core &core)
{
    static bool explosionApplied = false;

    if (explosionApplied)
        return;

    explosionApplied = true;

    glm::vec3 explosionCenter(0.0f, 5.0f, 0.0f);
    float explosionStrength = 50.0f;

    std::cout << "\n[Explosion] Applying radial impulses..." << std::endl;

    core.GetRegistry().view<DebrisTag, Physics::Component::RigidBody>().each(
        [&](Engine::Entity entity, const DebrisTag &tag, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
            glm::vec3 direction = glm::normalize(tag.position - explosionCenter);
            glm::vec3 impulse = direction * explosionStrength;

            Physics::Helper::AddImpulse(core, entity, impulse);

            std::cout << "  → Debris at (" << tag.position.x << ", " << tag.position.y << ", " << tag.position.z
                      << "): " << explosionStrength << "N·s impulse" << std::endl;
        });
}

static void PrintSummarySystem(Engine::Core &core)
{
    static int updateCount = 0;
    updateCount++;

    // Print summary after 120 updates (2 seconds at 60 FPS)
    if (updateCount == 120)
    {
        std::cout << "\n=================================================" << std::endl;
        std::cout << "  API Summary" << std::endl;
        std::cout << "=================================================\n" << std::endl;

        std::cout << "✓ AddForce()           : Continuous force at center" << std::endl;
        std::cout << "✓ AddForceAtPoint()    : Continuous force creating rotation" << std::endl;
        std::cout << "✓ AddTorque()          : Continuous rotational force" << std::endl;
        std::cout << "✓ AddImpulse()         : Instant velocity change" << std::endl;
        std::cout << "✓ AddImpulseAtPoint()  : Instant velocity + spin" << std::endl;
        std::cout << "✓ AddAngularImpulse()  : Instant angular velocity\n" << std::endl;

        std::cout << "All 6 API functions demonstrated successfully! ✅\n" << std::endl;

        // Stop the simulation
        core.Stop();
    }
}

static void CleanupSystem(Engine::Core &core)
{
    std::cout << "\nCleaning up Forces & Impulses example..." << std::endl;

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
// Plugin Registration
// ============================================================================

void ForcesImpulsesExample::Bind()
{
    RequirePlugins<Physics::Plugin>();
    RegisterSystems<Engine::Scheduler::Startup>(SetupSceneSystem);
    RegisterSystems<Engine::Scheduler::Startup>(ApplyExplosionImpulsesSystem);
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(ApplyContinuousForcesSystem);
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(PrintSummarySystem);
    RegisterSystems<Engine::Scheduler::Shutdown>(CleanupSystem);
}

} // namespace Examples
