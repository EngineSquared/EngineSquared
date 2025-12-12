/**************************************************************************
 * EngineSquared v0.1.1
 *
 * EngineSquared is a software package, part of the Engine² organization.
 *
 * This file is part of the EngineSquared project that is under MIT License.
 * Copyright © 2025-present by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the MIT License. See the project's LICENSE file for
 * the full license text and details.
 *
 * @file ConstraintInternal.hpp
 * @brief Internal constraint component for Jolt Physics data storage
 *
 * This component is automatically created/destroyed via entt hooks when
 * constraint components are added/removed. Users should never directly
 * interact with this component.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-12-04
 **************************************************************************/

#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/Constraints/Constraint.h>

namespace Physics::Component {

/**
 * @brief Enumeration of supported constraint types
 */
enum class ConstraintType : uint8_t {
    Fixed,    ///< Fixed constraint (0 DOF) - welds two bodies together
    Distance, ///< Distance constraint (5 DOF) - rope/spring with min/max distance
    Point     ///< Point constraint (3 DOF) - ball-and-socket joint
};

/**
 * @brief Internal component storing Jolt Physics constraint data
 *
 * This component is managed automatically by the ConstraintSystem
 * and should never be added/removed manually by users.
 *
 * @warning Internal use only! Do not use directly.
 */
struct ConstraintInternal {
    /// Pointer to the Jolt constraint object
    JPH::Constraint *constraint = nullptr;

    /// Type of the constraint for runtime identification
    ConstraintType type = ConstraintType::Fixed;

    /// Whether this constraint has been broken (exceeded force/torque thresholds)
    bool broken = false;

    /// Settings used to create this constraint (for force monitoring)
    float breakForce = 0.0f;
    float breakTorque = 0.0f;

    /**
     * @brief Default constructor (invalid constraint)
     */
    ConstraintInternal() = default;

    /**
     * @brief Construct with constraint pointer and type
     * @param c Pointer to the Jolt constraint
     * @param t Type of the constraint
     */
    ConstraintInternal(JPH::Constraint *c, ConstraintType t) : constraint(c), type(t) {}

    /**
     * @brief Construct with constraint pointer, type, and breaking thresholds
     * @param c Pointer to the Jolt constraint
     * @param t Type of the constraint
     * @param force Breaking force threshold
     * @param torque Breaking torque threshold
     */
    ConstraintInternal(JPH::Constraint *c, ConstraintType t, float force, float torque)
        : constraint(c), type(t), breakForce(force), breakTorque(torque)
    {
    }

    /**
     * @brief Check if this component has a valid constraint
     * @return true if the constraint pointer is valid
     */
    [[nodiscard]] bool IsValid() const { return constraint != nullptr; }

    /**
     * @brief Check if this constraint is breakable
     * @return true if either breakForce or breakTorque is greater than 0
     */
    [[nodiscard]] bool IsBreakable() const { return breakForce > 0.0f || breakTorque > 0.0f; }
};

} // namespace Physics::Component
