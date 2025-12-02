/**************************************************************************
 * KinematicMovementExample - Demonstration of Kinematic Movement API
 *
 * This example demonstrates how to use the Physics plugin with RigidBody
 * components to showcase kinematic movement features, including:
 * - MoveKinematic() for platform/elevator movement
 * - SetKinematicVelocity() for constant-speed movement
 * - Kinematic bodies carrying dynamic bodies
 * - Different movement patterns (linear, elevator, rotating)
 **************************************************************************/

#pragma once

#include "Engine.hpp"
#include "Physics.hpp"

namespace Examples {

/**
 * @brief Kinematic Movement Example Plugin
 *
 * Demonstrates:
 * - MoveKinematic() for platform/elevator movement
 * - SetKinematicVelocity() for constant-speed movement
 * - Kinematic bodies carrying dynamic bodies
 * - Different movement patterns (linear, elevator, rotating)
 */
class KinematicMovementExample : public Engine::APlugin {
  public:
    explicit KinematicMovementExample(Engine::Core &core) : APlugin(core) {}

    void Bind() override;

    const char *GetName() const { return "KinematicMovementExample"; }
    const char *GetVersion() const { return "1.0.0"; }
};

} // namespace Examples
