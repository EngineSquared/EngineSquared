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
 * @file PointConstraint.hpp
 * @brief Point constraint component (3 DOF) - ball-and-socket joint
 *
 * A point constraint connects two bodies at a single point, allowing free
 * rotation around that point but no relative translation. This is useful
 * for ragdolls, pendulums, ball joints, and articulated structures.
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
 * @brief Point constraint component (3 DOF)
 *
 * Connects two bodies at a point, allowing free rotation but no translation.
 * Also known as a ball-and-socket joint or spherical joint.
 *
 * Degrees of Freedom: 3 (rotation free, translation locked)
 *
 * @example Pendulum (body to world):
 * @code
 * // Create swinging pendulum attached to fixed point
 * auto constraint = Physics::Component::PointConstraint::CreateToWorld(
 *     pendulumWeight,
 *     glm::vec3(0, 5, 0),  // Fixed point in world space
 *     Physics::Component::ConstraintSettings::Rigid()
 * );
 *
 * pendulumWeight.AddComponent<Physics::Component::PointConstraint>(core, constraint);
 * @endcode
 *
 * @example Ragdoll joint:
 * @code
 * // Connect upper arm to shoulder
 * auto shoulderJoint = Physics::Component::PointConstraint::Create(
 *     torso,
 *     upperArm,
 *     glm::vec3(0.5f, 1.0f, 0),   // Shoulder position on torso
 *     glm::vec3(-0.2f, 0, 0),     // Top of upper arm
 *     Physics::Component::ConstraintSettings::Soft(0.8f, 0.2f)
 * );
 * @endcode
 *
 * @example Elbow joint:
 * @code
 * auto elbowJoint = Physics::Component::PointConstraint::Create(
 *     upperArm,
 *     lowerArm,
 *     glm::vec3(0.4f, 0, 0),   // End of upper arm
 *     glm::vec3(-0.4f, 0, 0),  // Start of lower arm
 *     Physics::Component::ConstraintSettings::Soft(0.9f, 0.1f)
 * );
 * @endcode
 */
struct PointConstraint {
    //========================================================================
    // Constraint Bodies
    //========================================================================

    /**
     * @brief First body entity (the entity this component is attached to)
     */
    Engine::EntityId bodyA = Engine::EntityId::Null();

    /**
     * @brief Second body entity (the entity to connect to)
     *
     * If invalid (default constructed), this becomes a world constraint.
     */
    Engine::EntityId bodyB = Engine::EntityId::Null();

    //========================================================================
    // Attachment Points
    //========================================================================

    /**
     * @brief Attachment point on bodyA in local body space
     *
     * The point on the first body where the joint is located.
     * Coordinates are relative to the body's center of mass.
     */
    glm::vec3 localPointA = glm::vec3(0.0f);

    /**
     * @brief Attachment point on bodyB in local body space (or world space if world constraint)
     *
     * For body-to-body constraints: local coordinates on bodyB
     * For world constraints: world space coordinates of the anchor point
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
     */
    bool broken = false;

    //========================================================================
    // Factory Methods
    //========================================================================

    /**
     * @brief Create a point constraint between two bodies
     *
     * @param a First body entity (owner of this component)
     * @param b Second body entity
     * @param pointA Local attachment point on bodyA
     * @param pointB Local attachment point on bodyB
     * @param constraintSettings Constraint behavior settings
     * @return Configured PointConstraint component
     */
    [[nodiscard]] static PointConstraint
    Create(Engine::EntityId a, Engine::EntityId b, const glm::vec3 &pointA = glm::vec3(0.0f),
           const glm::vec3 &pointB = glm::vec3(0.0f),
           const ConstraintSettings &constraintSettings = ConstraintSettings::Rigid())
    {
        PointConstraint constraint;
        constraint.bodyA = a;
        constraint.bodyB = b;
        constraint.localPointA = pointA;
        constraint.localPointB = pointB;
        constraint.settings = constraintSettings;
        constraint.broken = false;
        return constraint;
    }

    /**
     * @brief Create a point constraint from body to world (pendulum, anchor)
     *
     * Creates a ball-and-socket joint connecting a body to a fixed point
     * in world space. The body can rotate freely around this point.
     *
     * @param body The body entity to constrain
     * @param worldPoint Fixed anchor point in world space
     * @param constraintSettings Constraint behavior settings
     * @return Configured PointConstraint component
     */
    [[nodiscard]] static PointConstraint
    CreateToWorld(Engine::EntityId body, const glm::vec3 &worldPoint,
                  const ConstraintSettings &constraintSettings = ConstraintSettings::Rigid())
    {
        PointConstraint constraint;
        constraint.bodyA = body;
        constraint.bodyB = Engine::EntityId::Null(); // Invalid entity = world
        constraint.localPointA = glm::vec3(0.0f);    // Center of body
        constraint.localPointB = worldPoint;         // World position
        constraint.settings = constraintSettings;
        constraint.broken = false;
        return constraint;
    }

    /**
     * @brief Create a point constraint from body to world with local attachment
     *
     * Same as CreateToWorld, but allows specifying an offset on the body.
     *
     * @param body The body entity to constrain
     * @param worldPoint Fixed anchor point in world space
     * @param localPoint Attachment point on the body in local space
     * @param constraintSettings Constraint behavior settings
     * @return Configured PointConstraint component
     */
    [[nodiscard]] static PointConstraint
    CreateToWorldWithOffset(Engine::EntityId body, const glm::vec3 &worldPoint, const glm::vec3 &localPoint,
                            const ConstraintSettings &constraintSettings = ConstraintSettings::Rigid())
    {
        PointConstraint constraint;
        constraint.bodyA = body;
        constraint.bodyB = Engine::EntityId::Null(); // Invalid entity = world
        constraint.localPointA = localPoint;
        constraint.localPointB = worldPoint; // World position
        constraint.settings = constraintSettings;
        constraint.broken = false;
        return constraint;
    }

    /**
     * @brief Check if this is a world constraint
     * @return true if bodyB is invalid (world constraint)
     */
    [[nodiscard]] bool IsWorldConstraint() const { return bodyB.IsNull(); }
};

} // namespace Physics::Component
