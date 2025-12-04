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
#include <Jolt/Physics/Body/AllowedDOFs.h>
#include <Jolt/Physics/Body/MotionQuality.h>
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
 * @brief Motion quality for rigid bodies (Discrete or LinearCast)
 */
using MotionQuality = JPH::EMotionQuality;

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
    // Motion Quality / CCD
    //========================================================================
    /// Motion quality (Discrete or LinearCast / CCD)
    MotionQuality motionQuality = MotionQuality::Discrete;

    /// Alias for enabling continuous collision detection (LinearCast)
    bool useMotionQualityLinearCast = false;

    // Axis locks - translation / rotation locks, useful for 2D or constrained movement
    bool lockPositionX = false;
    bool lockPositionY = false;
    bool lockPositionZ = false;
    bool lockRotationX = false;
    bool lockRotationY = false;
    bool lockRotationZ = false;

    // Enhanced internal edge removal (boolean toggle)
    // When true, the engine will ask Jolt to perform additional checks to avoid ghost contacts
    // (may be more expensive). Default: false.
    bool enhancedInternalEdgeRemoval = false;

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
