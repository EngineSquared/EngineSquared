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
 * @file RigidBodyInternal.hpp
 * @brief Internal RigidBody component for Jolt Physics data storage
 *
 * This component is automatically created/destroyed via entt hooks when
 * RigidBody is added/removed. Users should never directly interact with
 * this component.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-10-28
 **************************************************************************/

#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyID.h>

namespace Physics::Component {

/**
 * @brief Internal component storing Jolt Physics data
 *
 * This component is managed automatically by the RigidBodySystem
 * and should never be added/removed manually by users.
 *
 * @warning Internal use only! Do not use directly.
 */
struct RigidBodyInternal {
    /// Jolt Physics body identifier
    JPH::BodyID bodyID;

    /**
     * @brief Default constructor (invalid body)
     */
    RigidBodyInternal() = default;

    /**
     * @brief Construct with body ID
     * @param id The Jolt body ID
     */
    explicit RigidBodyInternal(JPH::BodyID id) : bodyID(id) {}

    /**
     * @brief Check if this component has a valid body
     */
    [[nodiscard]] bool IsValid() const { return !bodyID.IsInvalid(); }
};

} // namespace Physics::Component
