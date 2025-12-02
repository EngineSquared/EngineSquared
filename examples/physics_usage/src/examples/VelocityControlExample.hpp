/**************************************************************************
 * VelocityControlExample - Demonstration of Velocity Control API
 *
 * This example demonstrates how to use the Physics plugin with RigidBody
 * components to showcase velocity control features, including:
 * - GetLinearVelocity / SetLinearVelocity / AddLinearVelocity
 * - GetAngularVelocity / SetAngularVelocity / AddAngularVelocity
 **************************************************************************/

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

    const char *GetName() const { return "VelocityControlExample"; }
    const char *GetVersion() const { return "1.0.0"; }
};

} // namespace Examples
