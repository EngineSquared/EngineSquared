/*
** EPITECH PROJECT, 2025
** EngineSquared - Physics Usage Examples
** File description:
** VelocityControlExample - Demonstrates Velocity Control API (Issue #002)
*/

#pragma once

#include "Engine.hpp"
#include "Physics.hpp"

namespace Examples {

/**
 * @brief Example plugin demonstrating Velocity Control API (Issue #002)
 *
 * Demonstrates all 6 velocity control functions:
 * - GetLinearVelocity / SetLinearVelocity / AddLinearVelocity
 * - GetAngularVelocity / SetAngularVelocity / AddAngularVelocity
 */
class VelocityControlExample : public Engine::APlugin {
  public:
    explicit VelocityControlExample(Engine::Core &core) : APlugin(core) {}

    void Bind() override;
};

} // namespace Examples
