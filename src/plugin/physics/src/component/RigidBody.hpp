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
 * @file RigidBody.hpp
 * @brief Public RigidBody component for Jolt Physics simulation
 *
 * This component encapsulates all Jolt Physics settings and provides
 * a clean API for users. Internal Jolt data is managed via RigidBodyInternal.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-10-28
 **************************************************************************/

#pragma once

#include "utils/Layers.hpp"
#include <Jolt/Physics/Body/MotionType.h>
#include <Jolt/Physics/EActivation.h>
#include <cstdint>

namespace Physics::Component {

/**
 * @brief Motion type for rigid bodies
 */
using MotionType = JPH::EMotionType;

/**
 * @brief Activation mode for bodies when added to the physics world
 */
using Activation = JPH::EActivation;

/**
 * @brief Public RigidBody component
 *
 * This component contains only the user-facing properties.
 * Internal Jolt data (BodyID, etc.) is stored in RigidBodyInternal
 * and managed automatically via entt hooks.
 *
 * @example Basic usage:
 * @code
 * Engine::Entity cube = core.CreateEntity();
 * cube.AddComponent<Object::Transform>(core, transform);
 * cube.AddComponent<Object::Mesh>(core, meshData);
 *
 * Physics::Component::RigidBody rigidBody;
 * rigidBody.motionType = Physics::Component::MotionType::Dynamic;
 * rigidBody.mass = 10.0f;
 * rigidBody.friction = 0.5f;
 * rigidBody.restitution = 0.3f;
 * cube.AddComponent<Physics::RigidBody>(core, rigidBody);
 * @endcode
 */
struct RigidBody {
    //========================================================================
    // Motion Properties
    //========================================================================

    /// Motion type (Static, Kinematic, or Dynamic)
    MotionType motionType = MotionType::Dynamic;

    /// Activation mode when added to world
    Activation activation = Activation::Activate;

    //========================================================================
    // Mass Properties
    //========================================================================

    /// Mass in kg (only for Dynamic bodies)
    float mass = 1.0f;

    /// Allow body to go to sleep when inactive
    bool allowSleeping = true;

    //========================================================================
    // Material Properties
    //========================================================================

    /// Friction coefficient [0, 1]
    float friction = 0.5f;

    /// Restitution/bounciness [0, 1]
    float restitution = 0.0f;

    /// Linear damping (resistance to linear motion)
    float linearDamping = 0.05f;

    /// Angular damping (resistance to rotation)
    float angularDamping = 0.05f;

    //========================================================================
    // Gravity
    //========================================================================

    /// Gravity factor (1.0 = normal gravity, 0.0 = no gravity)
    float gravityFactor = 1.0f;

    //========================================================================
    // Layer (for collision filtering)
    //========================================================================

    /// Object layer for collision filtering
    uint16_t objectLayer = 0; // 0 = MOVING layer by default

    //========================================================================
    // Helper Methods
    //========================================================================

    /**
     * @brief Create a static rigid body
     */
    static RigidBody CreateStatic()
    {
        RigidBody rb;
        rb.motionType = MotionType::Static;
        rb.activation = Activation::DontActivate;
        rb.objectLayer = Utils::Layers::NON_MOVING;
        rb.mass = 0.0f;
        return rb;
    }

    /**
     * @brief Create a kinematic rigid body
     */
    static RigidBody CreateKinematic()
    {
        RigidBody rb;
        rb.motionType = MotionType::Kinematic;
        rb.activation = Activation::Activate;
        rb.objectLayer = Utils::Layers::MOVING;
        rb.mass = 0.0f;
        return rb;
    }

    /**
     * @brief Create a dynamic rigid body
     * @param bodyMass Mass of the body in kg (default: 1.0)
     */
    static RigidBody CreateDynamic(float bodyMass = 1.0f)
    {
        RigidBody rb;
        rb.motionType = MotionType::Dynamic;
        rb.activation = Activation::Activate;
        rb.objectLayer = Utils::Layers::MOVING;
        rb.mass = bodyMass;
        return rb;
    }
};

} // namespace Physics::Component
