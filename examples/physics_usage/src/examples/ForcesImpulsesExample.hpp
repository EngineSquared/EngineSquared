/**************************************************************************
 * ForcesImpulsesExample - Demonstrates Force Applicator API
 *
 * This example demonstrates how to use the Physics plugin with RigidBody
 * components to showcase the Force Applicator API, including:
 * - AddForce()
 * - AddForceAtPoint()
 * - AddTorque()
 * - AddImpulse()
 * - AddImpulseAtPoint()
 * - AddAngularImpulse()
 **************************************************************************/

#pragma once

#include "Engine.hpp"
#include "Physics.hpp"

namespace Examples {

/**
 * @brief Plugin demonstrating the Force Applicator API (v0.3 - Issue #001)
 *
 * Features:
 * - AddForce() - Continuous force (thruster)
 * - AddForceAtPoint() - Force creating rotation (door push)
 * - AddTorque() - Continuous rotation (spinner)
 * - AddImpulse() - Instant velocity change (explosion)
 * - AddImpulseAtPoint() - Instant velocity + spin (bat hit)
 * - AddAngularImpulse() - Instant spin (flywheel)
 *
 * All 6 API functions are demonstrated with practical examples.
 */
class ForcesImpulsesExample : public Engine::APlugin {
public:
    explicit ForcesImpulsesExample(Engine::Core& core) : Engine::APlugin(core) {}
    ~ForcesImpulsesExample() override = default;

    void Bind() final;

    const char* GetName() const { return "ForcesImpulsesExample"; }
    const char* GetVersion() const { return "1.0.0"; }
};

} // namespace Examples
