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
     * @brief Check if body ID is valid
     * @return true if body ID is valid
     */
    [[nodiscard]] bool IsValid() const { return !bodyID.IsInvalid(); }
};

} // namespace Physics::Component
