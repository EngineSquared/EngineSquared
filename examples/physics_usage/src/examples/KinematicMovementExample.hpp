/*
** EPITECH PROJECT, 2025
** EngineSquared - Physics Usage Examples
** File description:
** KinematicMovementExample - Demonstration of Kinematic Movement API (Issue #003)
*/

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
};

} // namespace Examples
