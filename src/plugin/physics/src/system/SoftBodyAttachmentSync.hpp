/**************************************************************************
 * EngineSquared v0.2.0
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
 * @file SoftBodyAttachmentSync.hpp
 * @brief System for synchronizing SoftBody attachments to parent RigidBodies
 *
 * Handles the attachment of SoftBody entities to RigidBody parents (like vehicle chassis),
 * enabling deformable bodywork that follows the parent transform.
 *
 * @author @EngineSquared
 * @version 0.2.0
 * @date 2026-01-25
 **************************************************************************/

#pragma once

#include "core/Core.hpp"

namespace Physics::System {

/**
 * @brief Initialize the soft body attachment sync system
 *
 * Registers any necessary hooks for SoftBodyAttachment component.
 * Should be called during physics plugin initialization, after SoftBodySystem.
 *
 * @param core Reference to the engine core
 */
void InitSoftBodyAttachmentSync(Engine::Core &core);

/**
 * @brief Synchronize attached soft bodies to their parent transforms
 *
 * For each entity with SoftBody + SoftBodyAttachment:
 * 1. Get the parent RigidBody's current transform
 * 2. Update the anchor vertex positions to follow the parent
 * 3. Non-anchor vertices remain simulation-driven
 *
 * Should be called during FixedTimeUpdate, after PhysicsUpdate but before SyncSoftBodyVertices.
 *
 * @param core Reference to the engine core
 */
void SyncSoftBodyAttachments(Engine::Core &core);

} // namespace Physics::System
