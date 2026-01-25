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
 * @file MeshCollider.hpp
 * @brief Convex hull mesh collider component
 *
 * This component creates a convex hull collision shape from the
 * entity's Object::Mesh vertices using Jolt's ConvexHullShape.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2026-01-25
 **************************************************************************/

#pragma once

#include <glm/vec3.hpp>

namespace Physics::Component {

/**
 * @brief Convex hull mesh collider
 *
 * This component creates a convex hull collision shape from the entity's
 * Object::Mesh component vertices. The convex hull is computed automatically
 * by Jolt Physics from the mesh vertices.
 *
 * If this component is present on an entity with RigidBody, it uses the
 * mesh geometry for collision instead of requiring an explicit collider.
 *
 * @note The entity MUST have an Object::Mesh component for this to work.
 * @note Convex hulls are more expensive than primitives (Box, Sphere, Capsule)
 *       but much cheaper than concave mesh colliders.
 * @note Jolt automatically computes the convex hull from the provided points,
 *       so interior points are handled correctly.
 * @note The `offset` member is applied to the created physics shape.
 *
 * @example "Basic usage with a mesh entity"
 * @code
 * // Entity already has Object::Mesh component with vertices
 * Physics::Component::MeshCollider collider;
 * entity.AddComponent<Physics::Component::MeshCollider>(core, collider);
 *
 * // Add RigidBody - it will use the mesh vertices for collision
 * Physics::Component::RigidBody rigidBody = Physics::Component::RigidBody::CreateDynamic(10.0f);
 * entity.AddComponent<Physics::Component::RigidBody>(core, rigidBody);
 * @endcode
 */
struct MeshCollider {
    /// Local offset from entity transform (applied after convex hull creation)
    glm::vec3 offset{0.0f, 0.0f, 0.0f};

    /// Maximum convex radius (Jolt parameter for collision detection)
    /// Smaller values = sharper corners, larger values = smoother but less accurate
    float maxConvexRadius = 0.05f;

    /**
     * @brief Default constructor
     */
    MeshCollider() = default;

    /**
     * @brief Construct with offset
     * @param localOffset Local position offset from entity center
     */
    explicit MeshCollider(const glm::vec3 &localOffset) : offset(localOffset) {}

    /**
     * @brief Construct with offset and convex radius
     * @param localOffset Local position offset from entity center
     * @param convexRadius Maximum convex radius for collision detection
     */
    MeshCollider(const glm::vec3 &localOffset, float convexRadius)
        : offset(localOffset), maxConvexRadius(convexRadius) {}

    /**
     * @brief Create a mesh collider at an offset
     * @param localOffset Position offset from entity center
     * @return MeshCollider
     */
    static MeshCollider AtOffset(const glm::vec3 &localOffset) { return MeshCollider(localOffset); }
};

} // namespace Physics::Component
