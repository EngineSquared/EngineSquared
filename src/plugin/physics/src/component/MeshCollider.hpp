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
 */
struct MeshCollider {
    /// Maximum convex radius (Jolt parameter for collision detection)
    /// Smaller values = sharper corners, larger values = smoother but less accurate
    float maxConvexRadius = 0.05f;

    /**
     * @brief Default constructor
     */
    MeshCollider() = default;

    /**
     * @brief Construct with convex radius
     * @param convexRadius Maximum convex radius for collision detection
     */
    explicit MeshCollider(float convexRadius) : maxConvexRadius(convexRadius) {}
};

} // namespace Physics::Component
