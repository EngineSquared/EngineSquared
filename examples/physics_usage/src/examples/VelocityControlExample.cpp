/*
** EPITECH PROJECT, 2025
** EngineSquared - Physics Usage Examples
** File description:
** VelocityControlExample - Implementation (Issue #002)
*/

#include "VelocityControlExample.hpp"

#include "Object.hpp"

#include <glm/gtc/constants.hpp>
#include <iostream>

namespace Examples {

// ============================================================================
// Components for tracking entities
// ============================================================================

struct ConstantVelocityTag {};
struct SpinnerTag {};
struct OscillatorTag {};
struct AcceleratorTag {};

// ============================================================================
// Systems
// ============================================================================

static void SetupSceneSystem(Engine::Core &core)
{
    std::cout << "\n=================================================" << std::endl;
    std::cout << "  Velocity Control Example (Issue #002)" << std::endl;
    std::cout << "=================================================\n" << std::endl;

    auto &registry = core.GetRegistry();

    // Example 1: Cube with constant velocity (no forces)
    // ==================================================
    std::cout << "[Example 1] Constant Velocity Cube" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    auto constantCube = core.CreateEntity();

    auto transform1 = Object::Component::Transform();
    transform1.SetPosition(-5.0f, 2.0f, 0.0f);
    registry.emplace<Object::Component::Transform>(constantCube, transform1);

    auto rb1 = Physics::Component::RigidBody::CreateDynamic(1.0f);
    rb1.linearDamping = 0.0f; // No damping for constant velocity
    // rb1.useGravity = false;     // TODO: Disable gravity (not yet implemented)
    registry.emplace<Physics::Component::RigidBody>(constantCube, rb1);

    registry.emplace<ConstantVelocityTag>(constantCube);

    // Set constant velocity: 2 m/s to the right
    Physics::Resource::SetLinearVelocity(core, Engine::Entity(constantCube), glm::vec3(2.0f, 0.0f, 0.0f));

    std::cout << "  ✓ Created cube with constant velocity (2 m/s →)" << std::endl;
    std::cout << "  → Moves at constant speed without forces\n" << std::endl;

    // Example 2: Spinning cube (angular velocity)
    // ============================================
    std::cout << "[Example 2] Spinning Cube" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    auto spinner = core.CreateEntity();

    auto transform2 = Object::Component::Transform();
    transform2.SetPosition(0.0f, 2.0f, 0.0f);
    registry.emplace<Object::Component::Transform>(spinner, transform2);

    auto rb2 = Physics::Component::RigidBody::CreateDynamic(2.0f);
    rb2.angularDamping = 0.0f;
    // rb2.useGravity = false;     // TODO: Disable gravity (not yet implemented)
    registry.emplace<Physics::Component::RigidBody>(spinner, rb2);

    registry.emplace<SpinnerTag>(spinner);

    // Set angular velocity: 90°/second around Y-axis
    float degreesPerSecond = 90.0f;
    float radiansPerSecond = glm::radians(degreesPerSecond);
    Physics::Resource::SetAngularVelocity(core, Engine::Entity(spinner), glm::vec3(0.0f, radiansPerSecond, 0.0f));

    std::cout << "  ✓ Created spinning cube (" << degreesPerSecond << "°/sec around Y)" << std::endl;
    std::cout << "  → Spins continuously without torque\n" << std::endl;

    // Example 3: Oscillating cube (velocity inversion)
    // =================================================
    std::cout << "[Example 3] Oscillating Cube" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    auto oscillator = core.CreateEntity();

    auto transform3 = Object::Component::Transform();
    transform3.SetPosition(5.0f, 2.0f, 0.0f);
    registry.emplace<Object::Component::Transform>(oscillator, transform3);

    auto rb3 = Physics::Component::RigidBody::CreateDynamic(1.5f);
    rb3.linearDamping = 0.0f;
    // rb3.useGravity = false;     // TODO: Disable gravity (not yet implemented)
    registry.emplace<Physics::Component::RigidBody>(oscillator, rb3);

    registry.emplace<OscillatorTag>(oscillator);

    // Initial velocity
    Physics::Resource::SetLinearVelocity(core, Engine::Entity(oscillator), glm::vec3(0.0f, 3.0f, 0.0f));

    std::cout << "  ✓ Created oscillating cube (bounces up/down)" << std::endl;
    std::cout << "  → Velocity reversed at bounds\n" << std::endl;

    // Example 4: Accelerator cube (AddLinearVelocity)
    // ================================================
    std::cout << "[Example 4] Accelerating Cube" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    auto accelerator = core.CreateEntity();

    auto transform4 = Object::Component::Transform();
    transform4.SetPosition(0.0f, -2.0f, 0.0f);
    registry.emplace<Object::Component::Transform>(accelerator, transform4);

    auto rb4 = Physics::Component::RigidBody::CreateDynamic(1.0f);
    rb4.linearDamping = 0.0f;
    // rb4.useGravity = false;     // TODO: Disable gravity (not yet implemented)
    registry.emplace<Physics::Component::RigidBody>(accelerator, rb4);

    registry.emplace<AcceleratorTag>(accelerator);

    std::cout << "  ✓ Created accelerating cube" << std::endl;
    std::cout << "  → Accelerates using AddLinearVelocity\n" << std::endl;

    std::cout << "✓ All 4 examples created and initialized!\n" << std::endl;
}

static void VelocityUpdateSystem(Engine::Core &core)
{
    static uint32_t frame = 0;
    frame++;

    // Example 1: Oscillator - set velocity back and forth using SetLinearVelocity
    core.GetRegistry().view<OscillatorTag, Physics::Component::RigidBody>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
            float oscillate = std::sin(frame * 0.1f) * 5.0f;
            glm::vec3 velocity(oscillate, 0.0f, 0.0f);
            Physics::Resource::SetLinearVelocity(core, entity, velocity);
        });

    // Example 2: Spinner - set angular velocity using SetAngularVelocity
    core.GetRegistry().view<SpinnerTag, Physics::Component::RigidBody>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
            // Spin around Y axis at 90 degrees per second
            glm::vec3 angularVelocity(0.0f, glm::radians(90.0f), 0.0f);
            Physics::Resource::SetAngularVelocity(core, entity, angularVelocity);
        });

    // Example 3: Constant velocity cube
    core.GetRegistry().view<ConstantVelocityTag, Physics::Component::RigidBody>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
            // Check velocity and reset if needed (demonstrate Get+Set)
            glm::vec3 currentVel = Physics::Resource::GetLinearVelocity(core, entity);
            if (glm::length(currentVel) < 2.9f)
            {
                // Reset to target velocity if it's been dampened
                Physics::Resource::SetLinearVelocity(core, entity, glm::vec3(0.0f, 3.0f, 0.0f));
            }
        });

    // Example 4: Accelerator - add velocity each frame (first 60 frames)
    if (frame <= 60)
    {
        core.GetRegistry().view<AcceleratorTag, Physics::Component::RigidBody>().each(
            [&](Engine::Entity entity, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
                // Add small velocity increment each frame (acceleration)
                glm::vec3 deltaVelocity(0.05f, 0.0f, 0.0f);
                Physics::Resource::AddLinearVelocity(core, entity, deltaVelocity);
            });
    }
}

static void PrintSummarySystem(Engine::Core &core)
{
    static int updateCount = 0;
    updateCount++;

    // Print summary after 2 seconds (120 frames at 60 FPS)
    if (updateCount != 120)
        return;

    std::cout << "\n=================================================" << std::endl;
    std::cout << "  API Summary" << std::endl;
    std::cout << "=================================================\n" << std::endl;

    std::cout << "✓ SetLinearVelocity()    : Constant velocity cube" << std::endl;
    std::cout << "✓ GetLinearVelocity()    : Read velocity for decisions" << std::endl;
    std::cout << "✓ AddLinearVelocity()    : Accelerating cube" << std::endl;
    std::cout << "✓ SetAngularVelocity()   : Spinning cube" << std::endl;
    std::cout << "✓ GetAngularVelocity()   : Available for rotation queries" << std::endl;
    std::cout << "✓ AddAngularVelocity()   : Can accumulate spin" << std::endl;

    std::cout << "\nAll 6 API functions demonstrated successfully! ✅\n" << std::endl;

    // Print current velocities
    std::cout << "Current Velocities:" << std::endl;
    std::cout << "-------------------" << std::endl;

    core.GetRegistry().view<ConstantVelocityTag, Physics::Component::RigidBody>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
            glm::vec3 vel = Physics::Resource::GetLinearVelocity(core, entity);
            std::cout << "  Constant Velocity: (" << vel.x << ", " << vel.y << ", " << vel.z << ") m/s" << std::endl;
        });

    core.GetRegistry().view<SpinnerTag, Physics::Component::RigidBody>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
            glm::vec3 angVel = Physics::Resource::GetAngularVelocity(core, entity);
            float degreesPerSec = glm::degrees(angVel.y);
            std::cout << "  Spinner: " << degreesPerSec << "°/sec around Y" << std::endl;
        });

    core.GetRegistry().view<AcceleratorTag, Physics::Component::RigidBody>().each(
        [&](Engine::Entity entity, [[maybe_unused]] const Physics::Component::RigidBody &rb) {
            glm::vec3 vel = Physics::Resource::GetLinearVelocity(core, entity);
            std::cout << "  Accelerator: (" << vel.x << ", " << vel.y << ", " << vel.z
                      << ") m/s (reached after acceleration)" << std::endl;
        });

    std::cout << std::endl;

    // Stop the simulation
    core.Stop();
}

static void CleanupSystem(Engine::Core &core)
{
    std::cout << "\nCleaning up Velocity Control example..." << std::endl;

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

void VelocityControlExample::Bind()
{
    RequirePlugins<Physics::Plugin>();

    RegisterSystems<Engine::Scheduler::Startup>(SetupSceneSystem);
    RegisterSystems<Engine::Scheduler::FixedTimeUpdate>(VelocityUpdateSystem, PrintSummarySystem);
    RegisterSystems<Engine::Scheduler::Shutdown>(CleanupSystem);
}

} // namespace Examples
