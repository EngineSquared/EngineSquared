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
 * @file SphereCollider.hpp
 * @brief Sphere collision shape component
 *
 * User-specified sphere collider for physics simulation.
 * Spheres are the most efficient collision shape.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-12-05
 **************************************************************************/

#pragma once

#include <glm/vec3.hpp>

namespace Physics::Component {

/**
 * @brief Sphere-shaped collider
 *
 * Explicit sphere collider that users can add to customize collision shape.
 * If present on an entity with RigidBody, prevents DefaultCollider auto-creation.
 *
 * Spheres are the most efficient collision shape and should be preferred
 * when the object's shape can be approximated as a sphere.
 *
 * Common uses:
 * - Balls, projectiles
 * - Spherical triggers
 * - Particle systems
 * - Simplified character collision
 *
 * @note Spheres have uniform collision response in all directions
 * @note Most efficient primitive for broad phase and narrow phase collision detection
 * @note The `offset` member is applied to the created physics shape when building the collision shape.
 *
 * @example "Basic usage for a sphere with radius 0.5"
 * @code
 * Physics::Component::SphereCollider collider;
 * collider.radius = 0.5f;
 * entity.AddComponent<Physics::Component::SphereCollider>(core, collider);
 * @endcode
 */
struct SphereCollider {
    /// Radius of the sphere in world units
    float radius = 0.5f;

    /// Local offset from entity transform (center of sphere)
    glm::vec3 offset{0.0f, 0.0f, 0.0f};

    /**
     * @brief Default constructor (radius 0.5)
     */
    SphereCollider() = default;

    /**
     * @brief Construct with radius
     * @param r Sphere radius
     */
    explicit SphereCollider(float r) : radius(r) {}

    /**
     * @brief Construct with radius and offset
     * @param r Sphere radius
     * @param localOffset Local position offset from entity center
     */
    SphereCollider(float r, const glm::vec3 &localOffset) : radius(r), offset(localOffset) {}

    /**
     * @brief Get diameter of the sphere
     * @return Diameter (2 * radius)
     */
    [[nodiscard]] float GetDiameter() const { return radius * 2.0f; }

    /**
     * @brief Set diameter directly (converts to radius)
     * @param diameter Full diameter of sphere
     */
    void SetDiameter(float diameter) { radius = diameter * 0.5f; }

    /**
     * @brief Check if collider is valid
     * @return true if radius > 0
     */
    [[nodiscard]] bool IsValid() const { return radius > 0.0f; }

    //=========================================================================
    // Factory methods
    //=========================================================================

    /**
     * @brief Create a unit sphere (radius 1.0)
     * @return SphereCollider with radius 1.0
     */
    static SphereCollider Unit() { return SphereCollider(1.0f); }

    /**
     * @brief Create a sphere for a ball/projectile
     * @param r Radius of the ball
     * @return SphereCollider
     */
    static SphereCollider Ball(float r) { return SphereCollider(r); }

    /**
     * @brief Create a sphere at an offset (useful for compound shapes)
     * @param r Radius
     * @param localOffset Position offset from entity center
     * @return SphereCollider
     */
    static SphereCollider AtOffset(float r, const glm::vec3 &localOffset) { return SphereCollider(r, localOffset); }
};

} // namespace Physics::Component
