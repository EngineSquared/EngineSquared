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
 * @file SoftBodyAttachment.hpp
 * @brief Component to attach a SoftBody to a RigidBody parent
 *
 * This component enables a SoftBody to follow a RigidBody (like a vehicle chassis)
 * while still allowing deformation from collisions. The anchor vertices of the
 * SoftBody are constrained to follow the parent's transform.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2026-01-25
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include "entity/EntityId.hpp"

namespace Physics::Component {

/**
 * @brief Component to attach a SoftBody to a RigidBody parent
 *
 * When added to an entity that has both SoftBody and this component,
 * the SoftBodyAttachmentSync system will:
 * 1. Track the parent RigidBody's transform
 * 2. Update the anchor vertices to follow the parent
 * 3. Allow non-anchor vertices to deform naturally
 *
 * ## Usage
 *
 * @code
 * // Create chassis with RigidBody
 * auto chassis = builder.Build(core);
 *
 * // Create bodywork SoftBody
 * auto bodywork = core.CreateEntity();
 * bodywork.AddComponent<Transform>(core, position);
 * bodywork.AddComponent<Mesh>(core, bodyworkMesh);
 * bodywork.AddComponent<SoftBody>(core, SoftBodySettings::CarBodywork());
 *
 * // Attach to chassis
 * SoftBodyAttachment attachment;
 * attachment.parentEntity = chassis;
 * attachment.anchorVertices = {0, 1, 2, 3, ...};  // Vertices at attachment points
 * bodywork.AddComponent<SoftBodyAttachment>(core, attachment);
 * @endcode
 *
 * @see SoftBody
 * @see SoftBodySettings::CarBodywork()
 */
struct SoftBodyAttachment {
    //=========================================================================
    // Configuration
    //=========================================================================

    /// Entity ID of the parent RigidBody to follow
    Engine::EntityId parentEntity = Engine::EntityId::Null();

    /// Indices of vertices that are anchored to the parent (these follow rigidly)
    /// Non-anchor vertices can deform freely based on physics simulation
    std::vector<uint32_t> anchorVertices;

    /// Local offset from parent's transform origin
    /// Applied to the SoftBody's center position relative to parent
    glm::vec3 localOffset = glm::vec3(0.0f);

    /// Local rotation offset from parent
    glm::quat localRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    //=========================================================================
    // Sync options
    //=========================================================================

    /// Whether to sync position with parent each frame
    bool syncPosition = true;

    /// Whether to sync rotation with parent each frame
    bool syncRotation = true;

    /// Strength of anchor constraint [0, 1]
    /// 0 = anchors are completely free (no constraint)
    /// 1 = anchors rigidly follow parent (default)
    /// Values in between allow soft anchoring (some flexibility)
    float anchorStrength = 1.0f;

    //=========================================================================
    // Runtime state (managed by system)
    //=========================================================================

    /// Cached initial local positions of anchor vertices (relative to SoftBody origin)
    /// Populated automatically by SoftBodyAttachmentSync on first update
    std::vector<glm::vec3> anchorLocalPositions;

    /// Flag indicating if initial positions have been cached
    bool initialized = false;

    //=========================================================================
    // Methods
    //=========================================================================

    /**
     * @brief Check if this attachment is valid
     */
    [[nodiscard]] bool IsValid() const { return !parentEntity.IsNull() && !anchorVertices.empty(); }

    /**
     * @brief Get the number of anchor vertices
     */
    [[nodiscard]] size_t GetAnchorCount() const { return anchorVertices.size(); }

    /**
     * @brief Add an anchor vertex
     * @param vertexIndex Index of the vertex to anchor
     */
    void AddAnchor(uint32_t vertexIndex)
    {
        // Avoid duplicates
        for (uint32_t idx : anchorVertices)
        {
            if (idx == vertexIndex)
                return;
        }
        anchorVertices.push_back(vertexIndex);
    }

    /**
     * @brief Add multiple anchor vertices
     * @param indices Vector of vertex indices to anchor
     */
    void AddAnchors(const std::vector<uint32_t> &indices)
    {
        for (uint32_t idx : indices)
        {
            AddAnchor(idx);
        }
    }

    /**
     * @brief Clear all anchor vertices
     */
    void ClearAnchors()
    {
        anchorVertices.clear();
        anchorLocalPositions.clear();
        initialized = false;
    }

    //=========================================================================
    // Constructors
    //=========================================================================

    /**
     * @brief Default constructor
     */
    SoftBodyAttachment() = default;

    /**
     * @brief Construct with parent entity
     * @param parent Entity ID of the parent RigidBody
     */
    explicit SoftBodyAttachment(Engine::EntityId parent) : parentEntity(parent) {}

    /**
     * @brief Construct with parent and anchor vertices
     * @param parent Entity ID of the parent RigidBody
     * @param anchors Vector of vertex indices to anchor
     */
    SoftBodyAttachment(Engine::EntityId parent, const std::vector<uint32_t> &anchors)
        : parentEntity(parent), anchorVertices(anchors)
    {
    }

    /**
     * @brief Construct with parent, anchors, and offset
     * @param parent Entity ID of the parent RigidBody
     * @param anchors Vector of vertex indices to anchor
     * @param offset Local offset from parent transform
     */
    SoftBodyAttachment(Engine::EntityId parent, const std::vector<uint32_t> &anchors, const glm::vec3 &offset)
        : parentEntity(parent), anchorVertices(anchors), localOffset(offset)
    {
    }
};

} // namespace Physics::Component
