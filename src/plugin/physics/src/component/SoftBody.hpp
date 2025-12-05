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
 * @file SoftBody.hpp
 * @brief Soft body component for deformable physics objects
 *
 * Integrates with Jolt Physics SoftBody system for cloth, ropes, and
 * deformable objects with realistic physics simulation.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-12-05
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

namespace Physics::Component {

/**
 * @brief Type of soft body for preset configurations
 */
enum class SoftBodyType {
    Custom,   ///< User-defined configuration
    Cloth,    ///< 2D grid for flags, capes, curtains
    Rope,     ///< 1D chain for ropes, cables, chains
    Cube,     ///< 3D volumetric soft body
    Pressure, ///< Pressure-based soft body (balloons, inflatables)
};

/**
 * @brief Soft body simulation settings
 */
struct SoftBodySettings {
    //=========================================================================
    // Simulation parameters
    //=========================================================================

    /// Number of solver iterations (higher = more accurate but slower)
    uint32_t solverIterations = 5;

    /// Linear damping (velocity decay)
    float linearDamping = 0.1f;

    /// Maximum linear velocity (m/s)
    float maxLinearVelocity = 500.0f;

    /// Gravity factor (1.0 = normal gravity)
    float gravityFactor = 1.0f;

    //=========================================================================
    // Material properties
    //=========================================================================

    /// Restitution (bounciness) [0, 1]
    float restitution = 0.0f;

    /// Friction coefficient [0, 1]
    float friction = 0.2f;

    /// Pressure for volume preservation (0 = no pressure)
    /// For balloons/inflatables: n * R * T (substance amount * gas constant * temperature)
    float pressure = 0.0f;

    //=========================================================================
    // Constraint parameters
    //=========================================================================

    /// Edge constraint compliance (0 = rigid, higher = softer)
    float edgeCompliance = 0.0f;

    /// Shear constraint compliance
    float shearCompliance = 0.0f;

    /// Bend constraint compliance (FLT_MAX = disable bending)
    float bendCompliance = 1.0f;

    //=========================================================================
    // Collision
    //=========================================================================

    /// Vertex collision radius (for collision detection)
    float vertexRadius = 0.0f;

    /// Allow sleeping when stable
    bool allowSleeping = true;

    /// Treat faces as double-sided for collision
    bool doubleSidedFaces = false;

    //=========================================================================
    // Factory methods
    //=========================================================================

    /**
     * @brief Default soft body settings
     */
    static SoftBodySettings Default() { return {}; }

    /**
     * @brief Settings optimized for cloth simulation
     * @param stiffness Cloth stiffness [0, 1] (1 = stiff like canvas, 0 = very soft like silk)
     * @return SoftBodySettings
     */
    static SoftBodySettings Cloth(float stiffness = 0.5f)
    {
        SoftBodySettings s;
        s.solverIterations = 8;
        s.linearDamping = 0.2f;
        s.gravityFactor = 1.0f;
        s.friction = 0.3f;
        s.edgeCompliance = (1.0f - stiffness) * 0.001f;
        s.shearCompliance = (1.0f - stiffness) * 0.002f;
        s.bendCompliance = (1.0f - stiffness) * 0.5f;
        s.vertexRadius = 0.01f;
        s.doubleSidedFaces = true;
        return s;
    }

    /**
     * @brief Settings optimized for rope simulation
     * @param stiffness Rope stiffness [0, 1]
     * @return SoftBodySettings
     */
    static SoftBodySettings Rope(float stiffness = 0.9f)
    {
        SoftBodySettings s;
        s.solverIterations = 10;
        s.linearDamping = 0.3f;
        s.gravityFactor = 1.0f;
        s.friction = 0.5f;
        s.edgeCompliance = (1.0f - stiffness) * 0.0001f;
        s.shearCompliance = 1e10f; // Disable shear for 1D
        s.bendCompliance = (1.0f - stiffness) * 0.01f;
        s.vertexRadius = 0.02f;
        return s;
    }

    /**
     * @brief Settings for pressure-based soft bodies (balloons)
     * @param pressure Internal pressure
     * @return SoftBodySettings
     */
    static SoftBodySettings Balloon(float pressure = 1000.0f)
    {
        SoftBodySettings s;
        s.solverIterations = 8;
        s.linearDamping = 0.1f;
        s.pressure = pressure;
        s.restitution = 0.5f;
        s.friction = 0.2f;
        s.edgeCompliance = 0.001f;
        s.bendCompliance = 0.1f;
        s.vertexRadius = 0.02f;
        return s;
    }

    /**
     * @brief Settings for jelly-like objects
     * @return SoftBodySettings
     */
    static SoftBodySettings Jelly()
    {
        SoftBodySettings s;
        s.solverIterations = 5;
        s.linearDamping = 0.2f;
        s.restitution = 0.8f;
        s.friction = 0.1f;
        s.edgeCompliance = 0.01f;
        s.shearCompliance = 0.02f;
        s.bendCompliance = 0.5f;
        s.pressure = 500.0f;
        return s;
    }
};

/**
 * @brief Soft body component
 *
 * Represents a deformable physics object using Jolt's SoftBody system.
 * Soft bodies are made of vertices (particles) connected by constraints.
 *
 * Usage patterns:
 * 1. Cloth: 2D grid of vertices for flags, capes, curtains
 * 2. Rope: 1D chain of vertices for ropes, cables
 * 3. Volume: 3D mesh for deformable objects
 *
 * @example "Creating a simple cloth"
 * @code
 * auto softBody = SoftBody::CreateCloth(10, 10, 0.1f);
 * softBody.PinVertex(0);   // Pin top-left corner
 * softBody.PinVertex(9);   // Pin top-right corner
 * entity.AddComponent<SoftBody>(core, softBody);
 * @endcode
 */
struct SoftBody {
    //=========================================================================
    // Configuration
    //=========================================================================

    /// Type of soft body (affects default settings)
    SoftBodyType type = SoftBodyType::Custom;

    /// Simulation settings
    SoftBodySettings settings;

    //=========================================================================
    // Geometry definition (used by SoftBodySystem to create Jolt soft body)
    //=========================================================================

    /// Vertex positions in local space
    std::vector<glm::vec3> vertices;

    /// Vertex inverse masses (0 = pinned/fixed)
    std::vector<float> invMasses;

    /// Face indices (triangles, 3 indices per face)
    std::vector<uint32_t> faces;

    /// Edge constraints (pairs of vertex indices)
    std::vector<std::pair<uint32_t, uint32_t>> edges;

    /// Indices of pinned (fixed) vertices
    std::vector<uint32_t> pinnedVertices;

    //=========================================================================
    // Methods
    //=========================================================================

    /**
     * @brief Get number of vertices
     */
    [[nodiscard]] size_t GetVertexCount() const { return vertices.size(); }

    /**
     * @brief Get number of faces (triangles)
     */
    [[nodiscard]] size_t GetFaceCount() const { return faces.size() / 3; }

    /**
     * @brief Get number of edge constraints
     */
    [[nodiscard]] size_t GetEdgeCount() const { return edges.size(); }

    /**
     * @brief Pin a vertex (fix it in space)
     * @param vertexIndex Index of vertex to pin
     */
    void PinVertex(uint32_t vertexIndex)
    {
        if (vertexIndex < invMasses.size())
        {
            invMasses[vertexIndex] = 0.0f;
            pinnedVertices.push_back(vertexIndex);
        }
    }

    /**
     * @brief Unpin a vertex
     * @param vertexIndex Index of vertex to unpin
     * @param mass Mass to assign (default 1.0)
     */
    void UnpinVertex(uint32_t vertexIndex, float mass = 1.0f)
    {
        if (vertexIndex < invMasses.size())
        {
            invMasses[vertexIndex] = mass > 0.0f ? 1.0f / mass : 0.0f;
            pinnedVertices.erase(std::remove(pinnedVertices.begin(), pinnedVertices.end(), vertexIndex),
                                 pinnedVertices.end());
        }
    }

    /**
     * @brief Check if a vertex is pinned
     */
    [[nodiscard]] bool IsVertexPinned(uint32_t vertexIndex) const
    {
        return vertexIndex < invMasses.size() && invMasses[vertexIndex] == 0.0f;
    }

    /**
     * @brief Check if the soft body configuration is valid
     */
    [[nodiscard]] bool IsValid() const
    {
        return !vertices.empty() && vertices.size() == invMasses.size() && (faces.empty() || faces.size() % 3 == 0);
    }

    //=========================================================================
    // Factory methods
    //=========================================================================

    /**
     * @brief Create a cloth (2D grid)
     * @param width Number of vertices along X axis
     * @param height Number of vertices along Y axis
     * @param spacing Distance between vertices
     * @param stiffness Cloth stiffness [0, 1]
     * @return SoftBody configured as cloth
     */
    static SoftBody CreateCloth(uint32_t width, uint32_t height, float spacing = 0.1f, float stiffness = 0.5f);

    /**
     * @brief Create a rope (1D chain)
     * @param segmentCount Number of segments
     * @param segmentLength Length of each segment
     * @param stiffness Rope stiffness [0, 1]
     * @return SoftBody configured as rope
     */
    static SoftBody CreateRope(uint32_t segmentCount, float segmentLength = 0.1f, float stiffness = 0.9f);

    /**
     * @brief Create a volumetric cube
     * @param gridSize Number of vertices per axis
     * @param spacing Distance between vertices
     * @return SoftBody configured as cube
     */
    static SoftBody CreateCube(uint32_t gridSize, float spacing = 0.1f);

    /**
     * @brief Create from mesh data
     * @param verts Vertex positions
     * @param faceIndices Triangle face indices
     * @param settings Simulation settings
     * @return SoftBody with custom mesh
     */
    static SoftBody CreateFromMesh(const std::vector<glm::vec3> &verts, const std::vector<uint32_t> &faceIndices,
                                   const SoftBodySettings &settings = SoftBodySettings::Default());
};

} // namespace Physics::Component
