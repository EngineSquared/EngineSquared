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
 * @file DefaultCollider.hpp
 * @brief Default collider component auto-created when no explicit collider
 *
 * This collider is automatically created when a RigidBody is added to an
 * entity that has no explicit collider component (BoxCollider, etc.).
 * It creates a box shape based on the entity's mesh bounds.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-10-28
 **************************************************************************/

#pragma once

#include <glm/vec3.hpp>

namespace Physics::Component {

/**
 * @brief Default collider using mesh bounds
 *
 * This collider is automatically created if:
 * - Entity has RigidBody
 * - Entity has no BoxCollider, SphereCollider, or MeshCollider
 * - Entity has Object::Mesh component
 *
 * The box dimensions are calculated from the mesh bounds.
 *
 * @note Auto-managed by RigidBodySystem
 */
struct DefaultCollider {
    /// Half-extents of the box (size / 2)
    glm::vec3 halfExtents{0.5f, 0.5f, 0.5f};

    /// Local offset from entity transform
    glm::vec3 offset{0.0f, 0.0f, 0.0f};

    /**
     * @brief Default constructor (1x1x1 box)
     */
    DefaultCollider() = default;

    /**
     * @brief Construct with half-extents
     * @param extents Half the size in each dimension
     */
    explicit DefaultCollider(const glm::vec3 &extents) : halfExtents(extents) {}

    /**
     * @brief Construct with half-extents and offset
     * @param extents Half the size in each dimension
     * @param localOffset Local position offset
     */
    DefaultCollider(const glm::vec3 &extents, const glm::vec3 &localOffset) : halfExtents(extents), offset(localOffset)
    {
    }

    /**
     * @brief Get full dimensions of the box
     */
    [[nodiscard]] glm::vec3 GetSize() const { return halfExtents * 2.0f; }
};

} // namespace Physics::Component
