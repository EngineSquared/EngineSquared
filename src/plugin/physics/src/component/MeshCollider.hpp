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
 * @brief Triangle mesh collider (concave mesh support)
 *
 * This component creates a triangle mesh collision shape from the entity's
 * Object::Mesh component vertices and indices. This supports concave meshes,
 * making it ideal for static terrain, floors, and complex geometry.
 *
 * If this component is present on an entity with RigidBody, it uses the
 * mesh geometry for collision instead of requiring an explicit collider.
 *
 * @note The entity MUST have an Object::Mesh component for this to work.
 * @note Triangle mesh colliders should ONLY be used for STATIC objects.
 *       For dynamic objects, use primitive colliders or convex hulls.
 * @note The mesh scale from the Transform component is automatically applied.
 */
struct MeshCollider {
    /// Cosine of the angle threshold for active edges (default: cos(5°) ≈ 0.996)
    /// Active edges prevent objects from getting stuck on internal edges when sliding across triangles
    /// Lower values = more edges marked as active = smoother sliding
    float activeEdgeCosThresholdAngle = 0.996195f;

    /**
     * @brief Default constructor
     */
    MeshCollider() = default;

    /**
     * @brief Construct with custom active edge threshold
     * @param edgeThreshold Cosine of the angle threshold for active edges
     */
    explicit MeshCollider(float edgeThreshold) : activeEdgeCosThresholdAngle(edgeThreshold) {}
};

} // namespace Physics::Component
