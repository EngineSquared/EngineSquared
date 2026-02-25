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
 * @file SoftBodyInternal.hpp
 * @brief Internal Jolt soft body reference component
 *
 * Stores the Jolt BodyID for soft bodies created by SoftBodySystem.
 * This component is automatically added when a SoftBody is created.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-12-05
 **************************************************************************/

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <glm/glm.hpp>
#include <utility>
#include <vector>

namespace Physics::Component {

/**
 * @brief Internal component storing Jolt soft body reference
 *
 * Automatically created by SoftBodySystem when a SoftBody component is added.
 * Used to track the Jolt physics body for simulation and updates.
 *
 * @note Users should not create this component directly
 */
struct SoftBodyInternal {
    /// Jolt body ID for the soft body
    JPH::BodyID bodyID;

    /// Maps original mesh vertex index to deduplicated Jolt vertex index
    /// Used to sync Jolt simulation results back to the original mesh
    std::vector<uint32_t> vertexMap;

    /// Initial scale applied to vertices during creation
    /// Used to convert Jolt vertices (world-scale) back to local mesh space during sync
    glm::vec3 initialScale = glm::vec3(1.0f);

    /// Body ID of the rigid body this soft body is attached to (optional)
    /// When set, kinematic vertices will follow this body's transform
    JPH::BodyID attachedToBodyID;

    /// Indices of kinematic vertices in the Jolt soft body (deduplicated indices)
    /// These vertices will be moved to follow the attached body's transform
    std::vector<uint32_t> kinematicVertexIndices;

    /// Initial positions of kinematic vertices in local space (relative to attached body)
    /// Used to calculate world positions when the attached body moves
    std::vector<glm::vec3> kinematicVertexLocalPositions;

    /**
     * @brief Default constructor (invalid body)
     */
    SoftBodyInternal() : bodyID(JPH::BodyID()) {}

    /**
     * @brief Construct with body ID
     * @param id Jolt body ID
     */
    explicit SoftBodyInternal(JPH::BodyID id) : bodyID(id) {}

    /**
     * @brief Construct with body ID and vertex map
     * @param id Jolt body ID
     * @param map Vertex mapping from original to deduplicated indices
     */
    SoftBodyInternal(JPH::BodyID id, std::vector<uint32_t> map) : bodyID(id), vertexMap(std::move(map)) {}

    /**
     * @brief Construct with body ID, vertex map, and initial scale
     * @param id Jolt body ID
     * @param map Vertex mapping from original to deduplicated indices
     * @param scale Initial scale applied to vertices
     */
    SoftBodyInternal(JPH::BodyID id, std::vector<uint32_t> map, const glm::vec3 &scale)
        : bodyID(id), vertexMap(std::move(map)), initialScale(scale)
    {
    }

    /**
     * @brief Check if body ID is valid
     * @return true if body ID is valid
     */
    [[nodiscard]] bool IsValid() const { return !bodyID.IsInvalid(); }
};

} // namespace Physics::Component
