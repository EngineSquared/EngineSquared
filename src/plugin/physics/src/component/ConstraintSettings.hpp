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
 * @file ConstraintSettings.hpp
 * @brief Common settings structure for all constraint types
 *
 * This file provides the base configuration for physics constraints including
 * stiffness, damping, and breaking force/torque thresholds.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-12-04
 **************************************************************************/

#pragma once

#include <cstdint>
#include <limits>

namespace Physics::Component {

/**
 * @brief Common settings for all constraint types
 *
 * These settings control the physical behavior of constraints:
 * - Stiffness: How rigid the constraint is (1.0 = perfectly rigid)
 * - Damping: How much energy is absorbed (prevents oscillation)
 * - Breaking: Force/torque thresholds for destructible constraints
 *
 * @example Basic usage:
 * @code
 * // Rigid constraint (default)
 * auto settings = ConstraintSettings::Rigid();
 *
 * // Soft spring-like constraint
 * auto settings = ConstraintSettings::Soft(0.5f, 0.1f);
 *
 * // Breakable constraint (snaps at 100N force)
 * auto settings = ConstraintSettings::Breakable(100.0f, 50.0f);
 * @endcode
 */
struct ConstraintSettings {
    //========================================================================
    // Spring Properties
    //========================================================================

    /**
     * @brief Constraint stiffness [0.0, 1.0]
     *
     * Controls how rigid the constraint is:
     * - 1.0 = Perfectly rigid (no spring behavior)
     * - 0.5 = Medium stiffness (some flexibility)
     * - 0.0 = Very soft (minimal constraint force)
     *
     * Internally mapped to Jolt's SpringSettings::mStiffness
     */
    float stiffness = 1.0f;

    /**
     * @brief Constraint damping [0.0, 1.0]
     *
     * Controls how much energy is absorbed to prevent oscillation:
     * - 0.0 = No damping (bouncy, oscillates)
     * - 0.5 = Medium damping (some oscillation)
     * - 1.0 = Critical damping (no oscillation)
     *
     * Internally mapped to Jolt's SpringSettings::mDamping
     */
    float damping = 0.0f;

    //========================================================================
    // Breaking Thresholds
    //========================================================================

    /**
     * @brief Maximum force before constraint breaks (Newtons)
     *
     * When the constraint force exceeds this threshold, the constraint
     * is marked as broken and disabled.
     *
     * - 0.0 = Unbreakable (default)
     * - >0.0 = Breaks when force exceeds threshold
     *
     * @note Use infinity for truly unbreakable constraints
     */
    float breakForce = 0.0f;

    /**
     * @brief Maximum torque before constraint breaks (Newton-meters)
     *
     * When the constraint torque exceeds this threshold, the constraint
     * is marked as broken and disabled.
     *
     * - 0.0 = Unbreakable (default)
     * - >0.0 = Breaks when torque exceeds threshold
     *
     * @note Use infinity for truly unbreakable constraints
     */
    float breakTorque = 0.0f;

    //========================================================================
    // Collision Settings
    //========================================================================

    /**
     * @brief Enable collision between constrained bodies
     *
     * When true, the two bodies connected by this constraint can
     * still collide with each other. When false (default), collisions
     * between the constrained bodies are disabled.
     */
    bool enableCollision = false;

    //========================================================================
    // Factory Methods
    //========================================================================

    /**
     * @brief Create settings for a perfectly rigid constraint
     *
     * Rigid constraints have no spring behavior and cannot break.
     * Use for fixed attachments, welding, etc.
     *
     * @return ConstraintSettings configured for rigid behavior
     */
    [[nodiscard]] static ConstraintSettings Rigid()
    {
        ConstraintSettings settings;
        settings.stiffness = 1.0f;
        settings.damping = 0.0f;
        settings.breakForce = 0.0f;
        settings.breakTorque = 0.0f;
        return settings;
    }

    /**
     * @brief Create settings for a breakable constraint
     *
     * Breakable constraints are rigid until the force/torque exceeds
     * the specified thresholds, then they snap and are disabled.
     *
     * @param force Maximum force before breaking (Newtons)
     * @param torque Maximum torque before breaking (Newton-meters), defaults to 0 (force only)
     * @return ConstraintSettings configured for breakable behavior
     */
    [[nodiscard]] static ConstraintSettings Breakable(float force, float torque = 0.0f)
    {
        ConstraintSettings settings;
        settings.stiffness = 1.0f;
        settings.damping = 0.0f;
        settings.breakForce = force;
        settings.breakTorque = torque;
        return settings;
    }

    /**
     * @brief Create settings for a soft spring-like constraint
     *
     * Soft constraints have spring behavior and gradually enforce
     * the constraint over time. Use for ropes, springs, etc.
     *
     * @param stiff Stiffness coefficient [0.0, 1.0]
     * @param damp Damping coefficient [0.0, 1.0]
     * @return ConstraintSettings configured for soft behavior
     */
    [[nodiscard]] static ConstraintSettings Soft(float stiff = 0.5f, float damp = 0.1f)
    {
        ConstraintSettings settings;
        settings.stiffness = stiff;
        settings.damping = damp;
        settings.breakForce = 0.0f;
        settings.breakTorque = 0.0f;
        return settings;
    }

    /**
     * @brief Check if this constraint is breakable
     * @return true if either breakForce or breakTorque is greater than 0
     */
    [[nodiscard]] bool IsBreakable() const { return breakForce > 0.0f || breakTorque > 0.0f; }

    /**
     * @brief Check if this constraint is rigid (no spring behavior)
     * @return true if stiffness is 1.0 and damping is 0.0
     */
    [[nodiscard]] bool IsRigid() const { return stiffness >= 1.0f && damping <= 0.0f; }
};

} // namespace Physics::Component
