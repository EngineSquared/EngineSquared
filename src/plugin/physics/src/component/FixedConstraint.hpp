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
 * @file FixedConstraint.hpp
 * @brief Fixed constraint component (0 DOF) - welds two bodies together
 *
 * A fixed constraint removes all degrees of freedom between two bodies,
 * effectively welding them into a single rigid structure. This is useful
 * for compound objects, breakable structures, and rigid attachments.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-12-04
 **************************************************************************/

#pragma once

#include "ConstraintSettings.hpp"
#include <Engine.hpp>
#include <glm/glm.hpp>

namespace Physics::Component {

/**
 * @brief Fixed constraint component (0 DOF)
 *
 * Welds two rigid bodies together, removing all relative motion.
 * The bodies behave as a single rigid structure after connection.
 *
 * Degrees of Freedom: 0 (all translation and rotation locked)
 *
 * @example Two-body constraint:
 * @code
 * // Create two entities with RigidBody components
 * auto bodyA = core.CreateEntity();
 * auto bodyB = core.CreateEntity();
 * // ... add transforms and rigid bodies ...
 *
 * // Create fixed constraint between them
 * auto constraint = Physics::Component::FixedConstraint::Create(
 *     bodyA,
 *     bodyB,
 *     glm::vec3(0, -0.5f, 0),  // Attachment point on bodyA (local space)
 *     glm::vec3(0, 0.5f, 0),   // Attachment point on bodyB (local space)
 *     Physics::Component::ConstraintSettings::Rigid()
 * );
 *
 * bodyA.AddComponent<Physics::Component::FixedConstraint>(core, constraint);
 * @endcode
 *
 * @example Breakable fixed constraint:
 * @code
 * auto constraint = Physics::Component::FixedConstraint::Create(
 *     anchor,
 *     piece,
 *     glm::vec3(0, -0.5f, 0),
 *     glm::vec3(0, 0.5f, 0),
 *     Physics::Component::ConstraintSettings::Breakable(100.0f)  // Breaks at 100N
 * );
 * @endcode
 */
struct FixedConstraint {
    //========================================================================
    // Constraint Bodies
    //========================================================================

    /**
     * @brief First body entity (the entity this component is attached to)
     *
     * This should be the entity that owns this constraint component.
     * If bodyA is invalid, the constraint attaches to the world.
     */
    Engine::Entity bodyA;

    /**
     * @brief Second body entity (the entity to connect to)
     *
     * The other entity involved in the constraint.
     */
    Engine::Entity bodyB;

    //========================================================================
    // Attachment Points
    //========================================================================

    /**
     * @brief Attachment point on bodyA in local body space
     *
     * The point on the first body where the constraint attaches.
     * Coordinates are relative to the body's center of mass.
     */
    glm::vec3 localPointA = glm::vec3(0.0f);

    /**
     * @brief Attachment point on bodyB in local body space
     *
     * The point on the second body where the constraint attaches.
     * Coordinates are relative to the body's center of mass.
     */
    glm::vec3 localPointB = glm::vec3(0.0f);

    //========================================================================
    // Settings
    //========================================================================

    /**
     * @brief Constraint settings (stiffness, damping, breaking thresholds)
     */
    ConstraintSettings settings;

    //========================================================================
    // Runtime State
    //========================================================================

    /**
     * @brief Whether this constraint has been broken
     *
     * When a breakable constraint exceeds its force/torque threshold,
     * this flag is set to true and the Jolt constraint is disabled.
     * The component remains attached to allow user code to react.
     */
    bool broken = false;

    //========================================================================
    // Factory Methods
    //========================================================================

    /**
     * @brief Create a fixed constraint between two bodies
     *
     * @param a First body entity (owner of this component)
     * @param b Second body entity
     * @param pointA Local attachment point on bodyA
     * @param pointB Local attachment point on bodyB
     * @param constraintSettings Constraint behavior settings
     * @return Configured FixedConstraint component
     */
    [[nodiscard]] static FixedConstraint Create(
        Engine::Entity a,
        Engine::Entity b,
        const glm::vec3 &pointA = glm::vec3(0.0f),
        const glm::vec3 &pointB = glm::vec3(0.0f),
        const ConstraintSettings &constraintSettings = ConstraintSettings::Rigid())
    {
        FixedConstraint constraint;
        constraint.bodyA = a;
        constraint.bodyB = b;
        constraint.localPointA = pointA;
        constraint.localPointB = pointB;
        constraint.settings = constraintSettings;
        constraint.broken = false;
        return constraint;
    }

    /**
     * @brief Create a fixed constraint from one body to the world
     *
     * Attaches a body to a fixed point in world space.
     *
     * @param body The body entity to constrain
     * @param worldPoint Point in world space to attach to
     * @param constraintSettings Constraint behavior settings
     * @return Configured FixedConstraint component
     */
    [[nodiscard]] static FixedConstraint CreateToWorld(
        Engine::Entity body,
        const glm::vec3 &worldPoint,
        const ConstraintSettings &constraintSettings = ConstraintSettings::Rigid())
    {
        FixedConstraint constraint;
        constraint.bodyA = body;
        constraint.bodyB = Engine::Entity();  // Invalid entity = world
        constraint.localPointA = glm::vec3(0.0f);
        constraint.localPointB = worldPoint;  // Used as world position
        constraint.settings = constraintSettings;
        constraint.broken = false;
        return constraint;
    }

    /**
     * @brief Check if this is a world constraint (body to world, not body to body)
     * @return true if bodyB is invalid (world constraint)
     */
    [[nodiscard]] bool IsWorldConstraint() const
    {
        return !bodyB.IsValid();
    }
};

} // namespace Physics::Component
