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
 * @file DistanceConstraint.hpp
 * @brief Distance constraint component (5 DOF) - rope/spring with min/max distance
 *
 * A distance constraint maintains a specific distance between two bodies,
 * allowing rotation but restricting translation to a range. This is useful
 * for ropes, chains, springs, tethers, and bungee cords.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-12-04
 **************************************************************************/

#pragma once

#include "ConstraintSettings.hpp"
#include <Engine.hpp>
#include <glm/glm.hpp>
#include <utility>

namespace Physics::Component {

/**
 * @brief Distance constraint component (5 DOF)
 *
 * Maintains a distance range between two bodies while allowing free rotation.
 * The constraint has 5 degrees of freedom (3 rotational for each body connection
 * point, minus 1 for the distance constraint).
 *
 * Degrees of Freedom: 5 (rotation free, distance constrained)
 *
 * @example Simple rope:
 * @code
 * auto constraint = Physics::Component::DistanceConstraint::Create(
 *     anchorEntity,
 *     swingingEntity,
 *     3.0f,  // Fixed distance of 3 meters
 *     glm::vec3(0, -0.5f, 0),  // Bottom of anchor
 *     glm::vec3(0, 0.5f, 0),   // Top of swinging body
 *     Physics::Component::ConstraintSettings::Rigid()
 * );
 * @endcode
 *
 * @example Spring connection:
 * @code
 * auto constraint = Physics::Component::DistanceConstraint::CreateWithRange(
 *     bodyA,
 *     bodyB,
 *     1.0f,  // Minimum distance (compressed)
 *     3.0f,  // Maximum distance (extended)
 *     glm::vec3(0),
 *     glm::vec3(0),
 *     Physics::Component::ConstraintSettings::Soft(0.5f, 0.1f)
 * );
 * @endcode
 *
 * @example Chain link:
 * @code
 * auto constraint = Physics::Component::DistanceConstraint::Create(
 *     previousLink,
 *     currentLink,
 *     0.8f,  // Link length
 *     glm::vec3(0, -0.2f, 0),  // Bottom of previous link
 *     glm::vec3(0, 0.2f, 0),   // Top of current link
 *     Physics::Component::ConstraintSettings::Soft(0.9f, 0.1f)
 * );
 * @endcode
 */
struct DistanceConstraint {
    //========================================================================
    // Constraint Bodies
    //========================================================================

    /**
     * @brief First body entity (the entity this component is attached to)
     */
    Engine::Entity bodyA = Engine::Entity{};

    /**
     * @brief Second body entity (the entity to connect to)
     */
    Engine::Entity bodyB = Engine::Entity{}; // invalid => world (see IsWorldConstraint)

    //========================================================================
    // Attachment Points
    //========================================================================

    /**
     * @brief Attachment point on bodyA in local body space
     */
    glm::vec3 localPointA = glm::vec3(0.0f);

    /**
     * @brief Attachment point on bodyB in local body space (or world-space anchor for world constraints)
     *
     * For body-to-body constraints: local coordinates on bodyB.
     * For body-to-world constraints (when `bodyB` is invalid), this member stores a
     * world-space anchor point and is interpreted as such. Use `IsWorldConstraint()`
     * to detect this state.
     */
    glm::vec3 localPointB = glm::vec3(0.0f);

    //========================================================================
    // Distance Properties
    //========================================================================

    /**
     * @brief Minimum allowed distance between attachment points
     *
     * When the bodies try to get closer than this distance,
     * the constraint pushes them apart.
     *
     * Special values:
     * - -1.0f = Auto-detect from initial body positions
     * - 0.0f = Bodies can touch (no minimum)
     */
    float minDistance = -1.0f;

    /**
     * @brief Maximum allowed distance between attachment points
     *
     * When the bodies try to get further than this distance,
     * the constraint pulls them together.
     *
     * Special values:
     * - -1.0f = Auto-detect from initial body positions
     */
    float maxDistance = -1.0f;

    //========================================================================
    // Settings
    //========================================================================

    /**
     * @brief Constraint settings (stiffness, damping, breaking thresholds)
     */
    ConstraintSettings settings = ConstraintSettings::Rigid();

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
     * @brief Create a fixed-distance constraint (rope)
     *
     * Creates a constraint that maintains an exact distance between two bodies.
     *
     * @param a First body entity
     * @param b Second body entity
     * @param distance The fixed distance to maintain
     * @param pointA Local attachment point on bodyA
     * @param pointB Local attachment point on bodyB
     * @param constraintSettings Constraint behavior settings
     * @return Configured DistanceConstraint component
     */
    [[nodiscard]] static DistanceConstraint
    Create(Engine::Entity a, Engine::Entity b, float distance, const glm::vec3 &pointA = glm::vec3(0.0f),
           const glm::vec3 &pointB = glm::vec3(0.0f),
           const ConstraintSettings &constraintSettings = ConstraintSettings::Rigid())
    {
        DistanceConstraint constraint;
        constraint.bodyA = a;
        constraint.bodyB = b;
        constraint.localPointA = pointA;
        constraint.localPointB = pointB;
        constraint.minDistance = distance;
        constraint.maxDistance = distance;
        constraint.settings = constraintSettings;
        constraint.broken = false;
        return constraint;
    }

    /**
     * @brief Create a distance constraint with min/max range (spring)
     *
     * Creates a constraint that allows distance to vary within a range.
     *
     * @param a First body entity
     * @param b Second body entity
     * @param min Minimum distance (compressed state)
     * @param max Maximum distance (extended state)
     * @param pointA Local attachment point on bodyA
     * @param pointB Local attachment point on bodyB
     * @param constraintSettings Constraint behavior settings
     * @return Configured DistanceConstraint component
     */
    [[nodiscard]] static DistanceConstraint
    CreateWithRange(Engine::Entity a, Engine::Entity b, float min, float max, const glm::vec3 &pointA = glm::vec3(0.0f),
                    const glm::vec3 &pointB = glm::vec3(0.0f),
                    const ConstraintSettings &constraintSettings = ConstraintSettings::Soft())
    {
        if (min > max)
            std::swap(min, max);

        DistanceConstraint constraint;
        constraint.bodyA = a;
        constraint.bodyB = b;
        constraint.localPointA = pointA;
        constraint.localPointB = pointB;
        constraint.minDistance = min;
        constraint.maxDistance = max;
        constraint.settings = constraintSettings;
        constraint.broken = false;
        return constraint;
    }

    /**
     * @brief Create a distance constraint from body to world
     *
     * Attaches a body to a fixed point in world space with a distance constraint.
     *
     * @param body The body entity to constrain
     * @param worldPoint Point in world space
     * @param distance The distance to maintain from worldPoint
     * @param localPoint Attachment point on the body in local space
     * @param constraintSettings Constraint behavior settings
     * @return Configured DistanceConstraint component
     */
    [[nodiscard]] static DistanceConstraint
    CreateToWorld(Engine::Entity body, const glm::vec3 &worldPoint, float distance,
                  const glm::vec3 &localPoint = glm::vec3(0.0f),
                  const ConstraintSettings &constraintSettings = ConstraintSettings::Rigid())
    {
        DistanceConstraint constraint;
        constraint.bodyA = body;
        constraint.bodyB = Engine::Entity(); // Invalid entity = world
        constraint.localPointA = localPoint;
        constraint.localPointB = worldPoint; // Used as world position
        constraint.minDistance = distance;
        constraint.maxDistance = distance;
        constraint.settings = constraintSettings;
        constraint.broken = false;
        return constraint;
    }

    /**
     * @brief Create a distance constraint with auto-detected distance
     *
     * The distance is automatically calculated from the initial body positions.
     *
     * @param a First body entity
     * @param b Second body entity
     * @param pointA Local attachment point on bodyA
     * @param pointB Local attachment point on bodyB
     * @param constraintSettings Constraint behavior settings
     * @return Configured DistanceConstraint component
     */
    [[nodiscard]] static DistanceConstraint
    CreateAutoDistance(Engine::Entity a, Engine::Entity b, const glm::vec3 &pointA = glm::vec3(0.0f),
                       const glm::vec3 &pointB = glm::vec3(0.0f),
                       const ConstraintSettings &constraintSettings = ConstraintSettings::Rigid())
    {
        DistanceConstraint constraint;
        constraint.bodyA = a;
        constraint.bodyB = b;
        constraint.localPointA = pointA;
        constraint.localPointB = pointB;
        constraint.minDistance = -1.0f; // Auto-detect
        constraint.maxDistance = -1.0f; // Auto-detect
        constraint.settings = constraintSettings;
        constraint.broken = false;
        return constraint;
    }

    /**
     * @brief Check if this is a world constraint
     * @return true if bodyB is invalid (world constraint)
     */
    [[nodiscard]] bool IsWorldConstraint() const { return !bodyB.IsValid(); }

    /**
     * @brief Check if distances should be auto-detected
     * @return true if either min or max distance is -1.0f
     */
    [[nodiscard]] bool IsAutoDistance() const { return minDistance < 0.0f || maxDistance < 0.0f; }

    /**
     * @brief Check if this is a fixed distance (rope) or range (spring)
     * @return true if minDistance equals maxDistance
     */
    [[nodiscard]] bool IsFixedDistance() const { return minDistance == maxDistance; }
};

} // namespace Physics::Component
