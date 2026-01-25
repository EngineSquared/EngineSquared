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
 * @file BoxCollider.hpp
 * @brief Explicit box collider component
 *
 * User-specified box collider with customizable dimensions and offset.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-10-28
 **************************************************************************/

#pragma once

#include <glm/vec3.hpp>

namespace Physics::Component {

/**
 * @brief Box-shaped collider
 *
 * Explicit box collider that users can add to customize collision shape.
 * If present on an entity with RigidBody, overrides the automatic mesh-based collision.
 *
 * @note The `offset` member is applied to the created physics shape when building the collision shape.
 *
 * @example "Basic usage for a 1x2x1 box"
 * @code
 * Physics::Component::BoxCollider collider;
 * collider.halfExtents = glm::vec3(0.5f, 1.0f, 0.5f);
 * entity.AddComponent<Physics::BoxCollider>(core, collider);
 * @endcode
 */
struct BoxCollider {
    /// Half-extents of the box (size / 2)
    glm::vec3 halfExtents{0.5f, 0.5f, 0.5f};

    /// Local offset from entity transform
    glm::vec3 offset{0.0f, 0.0f, 0.0f};

    /// Convex radius for collision detection (smaller = sharper corners)
    float convexRadius = 0.05f;

    /**
     * @brief Default constructor (1x1x1 box)
     */
    BoxCollider() = default;

    /**
     * @brief Construct with half-extents
     * @param extents Half the size in each dimension
     */
    explicit BoxCollider(const glm::vec3 &extents) : halfExtents(extents) {}

    /**
     * @brief Construct with half-extents and offset
     * @param extents Half the size in each dimension
     * @param localOffset Local position offset
     */
    BoxCollider(const glm::vec3 &extents, const glm::vec3 &localOffset) : halfExtents(extents), offset(localOffset) {}

    /**
     * @brief Get full dimensions of the box
     */
    [[nodiscard]] glm::vec3 GetSize() const { return halfExtents * 2.0f; }

    /**
     * @brief Set size directly (converts to half-extents)
     * @param size Full size in each dimension
     */
    void SetSize(const glm::vec3 &size) { halfExtents = size * 0.5f; }
};

} // namespace Physics::Component
