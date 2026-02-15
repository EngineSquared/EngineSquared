/**************************************************************************
 * EngineSquared v0.2.0
 *
 * EngineSquared is a software package, part of the Engine² organization.
 *
 * This file is part of the EngineSquared project that is under MIT License.
 * Copyright © 2024-present by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the MIT License. See the project's LICENSE file for
 * the full license text and details.
 *
 * @file MeshSimplifier.hpp
 * @brief Mesh simplification utilities for reducing vertex count.
 *
 * Provides algorithms to reduce mesh complexity while preserving overall shape.
 * Useful for physics simulation where high vertex counts impact performance.
 *
 * @author @EngineSquared
 * @version 0.2.0
 * @date 2026-01-29
 **************************************************************************/

#pragma once

#include "component/Mesh.hpp"

#include <cstdint>
#include <optional>
#include <vector>

namespace Object::Utils {

/**
 * @brief Result of mesh simplification operation
 */
struct SimplificationResult {
    /// The simplified mesh
    Component::Mesh mesh;

    /// Original vertex count before simplification
    uint32_t originalVertexCount;

    /// Final vertex count after simplification
    uint32_t simplifiedVertexCount;

    /// Mapping from original vertex indices to simplified indices
    /// simplifiedIndex = vertexMap[originalIndex]
    /// Value of UINT32_MAX means the vertex was removed
    std::vector<uint32_t> vertexMap;

    /// Whether simplification was actually performed
    bool wasSimplified;
};

/**
 * @brief Settings for mesh simplification
 */
struct SimplificationSettings {
    /// Target maximum number of vertices
    /// If the mesh has fewer vertices than this, no simplification occurs
    uint32_t targetVertexCount = 2000;

    /// Minimum distance between vertices to consider them for merging
    /// Vertices closer than this will be merged
    float mergeDistance = 0.01f;

    /// Whether to preserve boundary edges (edges with only one adjacent face)
    bool preserveBoundaries = true;

    /// Whether to recalculate normals after simplification
    bool recalculateNormals = true;

    /// Aggressiveness of simplification [0, 1]
    /// 0 = minimal changes, 1 = aggressive reduction
    float aggressiveness = 0.5f;

    /**
     * @brief Default settings for vehicle chassis simplification
     * @return SimplificationSettings optimized for car bodies
     */
    static SimplificationSettings ForVehicleChassis()
    {
        SimplificationSettings s;
        s.targetVertexCount = 2000;
        s.mergeDistance = 0.02f;
        s.preserveBoundaries = true;
        s.recalculateNormals = true;
        s.aggressiveness = 0.6f;
        return s;
    }

    /**
     * @brief Settings for soft body physics
     * @param maxVertices Maximum vertices for physics simulation
     * @return SimplificationSettings
     */
    static SimplificationSettings ForSoftBody(uint32_t maxVertices = 1500)
    {
        SimplificationSettings s;
        s.targetVertexCount = maxVertices;
        s.mergeDistance = 0.015f;
        s.preserveBoundaries = false;
        s.recalculateNormals = true;
        s.aggressiveness = 0.7f;
        return s;
    }
};

/**
 * @brief Simplify a mesh to reduce vertex count
 *
 * This function reduces the number of vertices in a mesh while attempting
 * to preserve its overall shape. It uses vertex clustering and edge collapse
 * techniques.
 *
 * Algorithm overview:
 * 1. Build spatial hash grid for vertex proximity queries
 * 2. Identify vertex clusters within merge distance
 * 3. Collapse clusters to their centroid
 * 4. Rebuild triangle connectivity
 * 5. Remove degenerate triangles
 * 6. Optionally recalculate normals
 *
 * Implementation notes: The implementation uses a spatial hash for efficient
 * proximity queries and a union-find (disjoint set) structure to cluster
 * nearby vertices. Clusters are collapsed to centroids; triangles are rebuilt
 * and degenerate faces removed. Normals may be recomputed if requested. This
 * approach is fast and robust for large meshes but is lossy.
 *
 * @param mesh The input mesh to simplify
 * @param settings Simplification parameters
 * @return SimplificationResult containing the simplified mesh and metadata
 *
 * @note If the mesh has fewer vertices than targetVertexCount, the original
 *       mesh is returned unchanged with wasSimplified = false
 *
 * @note This is a lossy operation. Fine details may be lost.
 */
[[nodiscard]] SimplificationResult SimplifyMesh(const Component::Mesh &mesh,
                                                const SimplificationSettings &settings = {});

/**
 * @brief Iteratively simplify mesh until target vertex count is reached
 *
 * Unlike SimplifyMesh which does a single pass, this function will
 * progressively increase merge distance until the target is reached
 * or no more reduction is possible.
 *
 * @param mesh The input mesh to simplify
 * @param targetVertexCount Target maximum number of vertices
 * @param maxIterations Maximum number of simplification passes
 * @return SimplificationResult containing the simplified mesh
 */
[[nodiscard]] SimplificationResult SimplifyMeshToTarget(const Component::Mesh &mesh, uint32_t targetVertexCount,
                                                        uint32_t maxIterations = 10);

/**
 * @brief Simplify mesh using uniform grid voxelization
 *
 * This method divides the mesh bounding box into a uniform grid and
 * represents each occupied cell with a single vertex at the centroid
 * of all vertices in that cell. This preserves the overall shape better
 * than distance-based clustering for high reduction ratios.
 *
 * @param mesh The input mesh to simplify
 * @param targetVertexCount Approximate target vertex count
 * @return SimplificationResult containing the simplified mesh
 */
[[nodiscard]] SimplificationResult SimplifyMeshByVoxelization(const Component::Mesh &mesh, uint32_t targetVertexCount);

/**
 * @brief Check if a mesh needs simplification
 *
 * @param mesh The mesh to check
 * @param maxVertices Maximum allowed vertex count
 * @return true if mesh has more vertices than maxVertices
 */
[[nodiscard]] inline bool NeedsSimplification(const Component::Mesh &mesh, uint32_t maxVertices)
{
    return mesh.GetVertices().size() > maxVertices;
}

/**
 * @brief Deduplicate vertices in a mesh
 *
 * Merges vertices that are at the exact same position (within epsilon).
 * This is useful for meshes loaded from OBJ files where each face has
 * unique vertices even when they share the same position.
 *
 * @param mesh The input mesh
 * @param epsilon Distance tolerance for considering vertices identical
 * @return SimplificationResult with deduplicated mesh
 */
[[nodiscard]] SimplificationResult DeduplicateVertices(const Component::Mesh &mesh, float epsilon = 1e-6f);

/**
 * @brief Estimate the simplified vertex count without performing simplification
 *
 * Useful for deciding whether to use SoftBody or fall back to RigidBody
 * based on expected complexity.
 *
 * @param mesh The mesh to analyze
 * @param settings Simplification parameters
 * @return Estimated vertex count after simplification
 */
[[nodiscard]] uint32_t EstimateSimplifiedVertexCount(const Component::Mesh &mesh,
                                                     const SimplificationSettings &settings = {});

} // namespace Object::Utils
