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
    // Skinning data (updated each frame)
    //=========================================================================

    /// Joint matrices for skinning (relative to center of mass)
    /// For vehicle chassis, this is typically just 1 joint: the chassis transform
    std::vector<JPH::Mat44> jointMatrices;

    /// Number of joints (typically 1 for vehicle chassis)
    uint32_t numJoints = 1;

    /// Whether to hard skin all vertices this frame (for teleportation/reset)
    bool hardSkinNextFrame = true;

    //=========================================================================
    // State tracking
    //=========================================================================

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
    SoftBodyChassisInternal() : skeletonBodyID(JPH::BodyID()), softBodyID(JPH::BodyID())
    {
        // Initialize with identity joint matrix
        jointMatrices.push_back(JPH::Mat44::sIdentity());
    }

    /**
     * @brief Construct with body IDs
     * @param skeleton RigidBody skeleton ID
     * @param softBody SoftBody ID
     */
    SoftBodyChassisInternal(JPH::BodyID skeleton, JPH::BodyID softBody) : skeletonBodyID(skeleton), softBodyID(softBody)
    {
        jointMatrices.push_back(JPH::Mat44::sIdentity());
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
        jointMatrices.push_back(JPH::Mat44::sIdentity());
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
    // Skinning helpers
    //=========================================================================

    /**
     * @brief Request hard skinning for next frame
     *
     * Call this when the vehicle is teleported or reset to immediately
     * snap all vertices to their skinned positions.
     */
    void RequestHardSkin() { hardSkinNextFrame = true; }

    /**
     * @brief Update joint matrix from skeleton transform
     *
     * This should be called each frame before SkinVertices() to update
     * the joint matrix with the current skeleton position/rotation.
     *
     * @param localTransform Transform relative to center of mass
     */
    void UpdateJointMatrix(const JPH::Mat44 &localTransform)
    {
        if (!jointMatrices.empty())
        {
            jointMatrices[0] = localTransform;
        }
    }
};

} // namespace Physics::Component
