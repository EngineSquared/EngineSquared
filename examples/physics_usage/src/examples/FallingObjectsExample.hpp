/**************************************************************************
 * FallingObjectsExample - Original physics demonstration with falling cubes
 *
 * This example demonstrates how to use the Physics plugin with RigidBody
 * components to create a simple physics simulation with falling cubes.
 **************************************************************************/

#pragma once

#include "Engine.hpp"
#include "Event.hpp"
#include "Physics.hpp"
#include "event/CollisionEvent.hpp"

namespace Examples {

/**
 * @brief Plugin demonstrating basic physics simulation
 *
 * Features:
 * - Static floor
 * - Dynamic falling cubes with different masses
 * - Bouncy ball with high restitution
 * - Kinematic moving platform
 * - Automatic simulation loop (100 steps)
 */
class FallingObjectsExample : public Engine::APlugin {
  public:
    explicit FallingObjectsExample(Engine::Core &core) : Engine::APlugin(core) {}
    ~FallingObjectsExample() override = default;

    void Bind() final;

    const char *GetName() const { return "FallingObjectsExample"; }
    const char *GetVersion() const { return "1.0.0"; }
};

} // namespace Examples
