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
 * @file SoftBodyChassisInternal.hpp
 * @brief Internal component for soft body chassis Jolt references.
 *
 * Stores the Jolt BodyIDs and skinning data for the hybrid architecture:
 * - RigidBody skeleton (invisible, handles physics)
 * - SoftBody mesh (visible, deformable)
 *
 * @author @EngineSquared
 * @version 0.2.0
 * @date 2026-01-25
 **************************************************************************/

#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Math/Mat44.h>
#include <Jolt/Physics/Body/BodyID.h>
#include <glm/glm.hpp>
#include <vector>

namespace Physics::Component {

/**
 * @brief Internal component storing Jolt references for soft body chassis
 *
 * This component is automatically created by SoftBodyChassisSystem when
 * a SoftBodyChassis component is added to an entity with a Vehicle component.
 *
 * It maintains the dual-body architecture:
 * - skeletonBodyID: The invisible RigidBody that VehicleConstraint attaches to
 * - softBodyID: The visible SoftBody that renders and deforms
 *
 * The skinning system updates the SoftBody vertices to follow the skeleton
 * while allowing localized deformation from collisions.
 *
 * @note Users should not create or modify this component directly
 */
struct SoftBodyChassisInternal {
    //=========================================================================
    // Jolt Body IDs
    //=========================================================================

    /// Body ID of the invisible RigidBody skeleton
    /// This is what VehicleConstraint is attached to
    JPH::BodyID skeletonBodyID;

    /// Body ID of the visible SoftBody
    /// This provides the deformable visual mesh
    JPH::BodyID softBodyID;

    //=========================================================================
    // Vertex mapping
    //=========================================================================

    /// Maps original mesh vertex index to deduplicated SoftBody vertex index
    /// softBodyVertexIdx = vertexMap[originalMeshVertexIdx]
    std::vector<uint32_t> vertexMap;

    /// Initial scale applied to vertices during creation
    /// Used to convert between world space and local mesh space
    glm::vec3 initialScale = glm::vec3(1.0f);

    //=========================================================================
    // Anchor data (for attachment-based deformation)
    //=========================================================================

    /// Indices of vertices that are anchored (in Jolt's deduplicated space)
    /// These vertices are forced to their original local positions each frame
    std::vector<uint32_t> anchorVertexIndices;

    /// Original local positions of anchor vertices (in scaled space)
    /// Used to reset anchors to their correct positions relative to chassis
    std::vector<glm::vec3> anchorLocalPositions;

    /// Whether anchors have been initialized
    bool anchorsInitialized = false;

    //=========================================================================
    // State tracking
    //=========================================================================

    /// Whether to teleport all vertices this frame (for initialization/reset)
    bool hardSkinNextFrame = true;

    /// Whether the soft body was successfully initialized
    bool isInitialized = false;

    /// Whether simplification was applied to the mesh
    bool wasSimplified = false;

    /// Original vertex count before simplification
    uint32_t originalVertexCount = 0;

    /// Simplified vertex count used by SoftBody
    uint32_t simplifiedVertexCount = 0;

    //=========================================================================
    // Constructors
    //=========================================================================

    /**
     * @brief Default constructor (invalid bodies)
     */
    SoftBodyChassisInternal() : skeletonBodyID(JPH::BodyID()), softBodyID(JPH::BodyID()) {}

    /**
     * @brief Construct with body IDs
     * @param skeleton RigidBody skeleton ID
     * @param softBody SoftBody ID
     */
    SoftBodyChassisInternal(JPH::BodyID skeleton, JPH::BodyID softBody) : skeletonBodyID(skeleton), softBodyID(softBody)
    {
    }

    /**
     * @brief Construct with body IDs and vertex map
     * @param skeleton RigidBody skeleton ID
     * @param softBody SoftBody ID
     * @param map Vertex mapping from original to deduplicated indices
     */
    SoftBodyChassisInternal(JPH::BodyID skeleton, JPH::BodyID softBody, std::vector<uint32_t> map)
        : skeletonBodyID(skeleton), softBodyID(softBody), vertexMap(std::move(map))
    {
    }

    //=========================================================================
    // Validation
    //=========================================================================

    /**
     * @brief Check if skeleton body ID is valid
     * @return true if skeleton body ID is valid
     */
    [[nodiscard]] bool HasValidSkeleton() const { return !skeletonBodyID.IsInvalid(); }

    /**
     * @brief Check if soft body ID is valid
     * @return true if soft body ID is valid
     */
    [[nodiscard]] bool HasValidSoftBody() const { return !softBodyID.IsInvalid(); }

    /**
     * @brief Check if both bodies are valid and system is ready
     * @return true if fully initialized
     */
    [[nodiscard]] bool IsValid() const { return HasValidSkeleton() && HasValidSoftBody() && isInitialized; }

    //=========================================================================
    // Anchor helpers
    //=========================================================================

    /**
     * @brief Request hard sync for next frame
     *
     * Call this when the vehicle is teleported or reset to immediately
     * snap all vertices to their original positions.
     */
    void RequestHardSkin() { hardSkinNextFrame = true; }
};

} // namespace Physics::Component
