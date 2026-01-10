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
 * @file CapsuleCollider.hpp
 * @brief Capsule collision shape component
 *
 * User-specified capsule collider for physics simulation.
 * Capsules are ideal for characters and cylindrical objects.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-12-05
 **************************************************************************/

#pragma once

#include <glm/vec3.hpp>

namespace Physics::Component {

/**
 * @brief Capsule-shaped collider (cylinder with hemispherical caps)
 *
 * Explicit capsule collider that users can add to customize collision shape.
 * If present on an entity with RigidBody, prevents DefaultCollider auto-creation.
 *
 * A capsule is defined as a line segment with a radius, creating a shape
 * that looks like a cylinder with rounded (hemispherical) ends.
 *
 * The capsule is oriented along the Y-axis by default:
 * - One hemisphere cap at (0, -halfHeight, 0)
 * - One hemisphere cap at (0, +halfHeight, 0)
 *
 * Total height = 2 * halfHeight + 2 * radius
 *
 * Common uses:
 * - Character controllers
 * - Humanoid bodies
 * - Cylindrical objects (cans, pipes, limbs)
 * - Projectiles with elongated shapes
 *
 * @note Capsules roll smoothly and are more stable than cylinders
 * @note Very efficient for collision detection
 *
 * @example "Basic usage for a character capsule"
 * @code
 * Physics::Component::CapsuleCollider collider;
 * collider.halfHeight = 0.8f;  // Cylinder part is 1.6m tall
 * collider.radius = 0.3f;      // Total height = 1.6 + 0.6 = 2.2m
 * entity.AddComponent<Physics::Component::CapsuleCollider>(core, collider);
 * @endcode
 */
struct CapsuleCollider {
    /// Half height of the cylindrical part (distance from center to cap start)
    float halfHeight = 0.5f;

    /// Radius of the capsule (both cylinder and hemisphere caps)
    float radius = 0.25f;

    /// Local offset from entity transform (center of capsule)
    glm::vec3 offset{0.0f, 0.0f, 0.0f};

    /**
     * @brief Default constructor
     */
    CapsuleCollider() = default;

    /**
     * @brief Construct with half-height and radius
     * @param hHeight Half height of the cylindrical part
     * @param r Radius of the capsule
     */
    CapsuleCollider(float hHeight, float r) : halfHeight(hHeight), radius(r) {}

    /**
     * @brief Construct with half-height, radius and offset
     * @param hHeight Half height of the cylindrical part
     * @param r Radius of the capsule
     * @param localOffset Local position offset from entity center
     */
    CapsuleCollider(float hHeight, float r, const glm::vec3 &localOffset)
        : halfHeight(hHeight), radius(r), offset(localOffset)
    {
    }

    /**
     * @brief Get total height of the capsule
     * @return Total height (2 * halfHeight + 2 * radius)
     */
    [[nodiscard]] float GetTotalHeight() const { return (halfHeight + radius) * 2.0f; }

    /**
     * @brief Get height of just the cylindrical part
     * @return Cylinder height (2 * halfHeight)
     */
    [[nodiscard]] float GetCylinderHeight() const { return halfHeight * 2.0f; }

    /**
     * @brief Get diameter of the capsule
     * @return Diameter (2 * radius)
     */
    [[nodiscard]] float GetDiameter() const { return radius * 2.0f; }

    /**
     * @brief Set height (distributes between cylinder and caps)
     * @param capsuleHeight Total height of the capsule
     *
     * @note This keeps the current radius and adjusts halfHeight accordingly
     */
    void SetHeight(float capsuleHeight)
    {
        halfHeight = (capsuleHeight * 0.5f) - radius;
        if (halfHeight < 0.0f)
            halfHeight = 0.0f;
    }
    /**
     * @brief Check if collider is valid
     * @return true if halfHeight >= 0 and radius > 0
     */
    [[nodiscard]] bool IsValid() const { return halfHeight >= 0.0f && radius > 0.0f; }

    /**
     * @brief Check if this capsule degenerates to a sphere
     * @return true if halfHeight == 0
     */
    [[nodiscard]] bool IsSphere() const { return halfHeight == 0.0f; }

    /**
     * @brief Create a capsule from total height and radius
     * @param capsuleHeight Total height including caps
     * @param r Width/depth radius
     * @return CapsuleCollider
     *
     * @note By default capsules are vertical (Y-up). Use rotation in Transform
     *       to make it horizontal, or use offset to position it differently.
     * @note Can be used for Character, common values: height 1.8m, radius 0.3m
     */
    static CapsuleCollider CreateFromHeight(float capsuleHeight, float r)
    {
        float hHeight = (capsuleHeight * 0.5f) - r;
        return CapsuleCollider(hHeight > 0.0f ? hHeight : 0.0f, r);
    }

    /**
     * @brief Create a capsule at an offset (useful for compound shapes)
     * @param hHeight Half height
     * @param r Radius
     * @param localOffset Position offset from entity center
     * @return CapsuleCollider
     */
    static CapsuleCollider AtOffset(float hHeight, float r, const glm::vec3 &localOffset)
    {
        return CapsuleCollider(hHeight, r, localOffset);
    }
};

} // namespace Physics::Component
