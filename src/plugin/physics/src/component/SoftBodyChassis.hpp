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
 * @file SoftBodyChassis.hpp
 * @brief Soft body chassis component for deformable vehicle bodies.
 *
 * Enables vehicle chassis to deform on collision while maintaining
 * physics integrity through a hybrid RigidBody skeleton + SoftBody visual system.
 *
 * Architecture:
 * - Invisible RigidBody handles VehicleConstraint physics
 * - Visible SoftBody provides deformable visual mesh
 * - Skinned constraints synchronize SoftBody to RigidBody transform
 *
 * @author @EngineSquared
 * @version 0.2.0
 * @date 2026-01-25
 **************************************************************************/

#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace Physics::Component {

/**
 * @brief Settings for soft body chassis behavior
 */
struct SoftBodyChassisSettings {
    //=========================================================================
    // Deformation parameters
    //=========================================================================

    /// Maximum distance vertices can move from their skinned position (meters)
    /// Higher values allow more visible deformation
    float maxDeformation = 0.15f;

    /// Stiffness of the soft body [0, 1]
    /// 1 = very stiff (minimal deformation), 0 = very soft
    float stiffness = 0.85f;

    /// Whether deformation persists after collision
    /// true = permanent dents, false = elastic return to shape
    bool permanentDeformation = true;

    /// Recovery rate when permanentDeformation is false [0, 1]
    /// How quickly the body returns to original shape (per second)
    float recoveryRate = 0.5f;

    //=========================================================================
    // Mesh simplification
    //=========================================================================

    /// Maximum vertices for SoftBody simulation
    /// Meshes with more vertices will be automatically simplified
    uint32_t maxVertices = 2000;

    /// Whether to automatically simplify meshes exceeding maxVertices
    bool autoSimplify = true;

    /// Fall back to RigidBody if mesh cannot be simplified enough
    bool fallbackToRigidBody = true;

    //=========================================================================
    // Physics parameters
    //=========================================================================

    /// Number of solver iterations (higher = more accurate but slower)
    uint32_t solverIterations = 6;

    /// Linear damping to reduce oscillations
    float linearDamping = 0.3f;

    /// Collision radius for vertices (meters)
    float vertexRadius = 0.02f;

    /// Friction coefficient for soft body collisions
    float friction = 0.4f;

    //=========================================================================
    // Skinning parameters
    //=========================================================================

    /// Back-stop distance to prevent vertices from penetrating the skeleton
    float backStopDistance = 0.05f;

    /// Back-stop sphere radius
    float backStopRadius = 0.1f;

    //=========================================================================
    // Factory methods
    //=========================================================================

    /**
     * @brief Default settings for car chassis
     * @return SoftBodyChassisSettings optimized for cars
     */
    static SoftBodyChassisSettings Default() { return {}; }

    /**
     * @brief Settings for realistic deformation (racing simulation)
     * @return SoftBodyChassisSettings with permanent deformation
     */
    static SoftBodyChassisSettings Realistic()
    {
        SoftBodyChassisSettings s;
        s.maxDeformation = 0.2f;
        s.stiffness = 0.8f;
        s.permanentDeformation = true;
        s.maxVertices = 2500;
        s.solverIterations = 8;
        return s;
    }

    /**
     * @brief Settings for arcade-style elastic deformation
     * @return SoftBodyChassisSettings with elastic recovery
     */
    static SoftBodyChassisSettings Arcade()
    {
        SoftBodyChassisSettings s;
        s.maxDeformation = 0.25f;
        s.stiffness = 0.7f;
        s.permanentDeformation = false;
        s.recoveryRate = 2.0f;
        s.maxVertices = 1500;
        s.solverIterations = 4;
        return s;
    }

    /**
     * @brief High-performance settings (lower quality, better FPS)
     * @return SoftBodyChassisSettings optimized for performance
     */
    static SoftBodyChassisSettings Performance()
    {
        SoftBodyChassisSettings s;
        s.maxDeformation = 0.1f;
        s.stiffness = 0.9f;
        s.permanentDeformation = true;
        s.maxVertices = 1000;
        s.solverIterations = 4;
        s.linearDamping = 0.5f;
        return s;
    }
};

/**
 * @brief Soft body chassis component for deformable vehicle bodies
 *
 * This component enables vehicle chassis to visually deform on collision
 * while maintaining proper physics simulation through VehicleConstraint.
 *
 * The system uses a hybrid architecture:
 * 1. An invisible RigidBody (skeleton) attached to VehicleConstraint
 * 2. A visible SoftBody (chassis mesh) skinned to the RigidBody
 * 3. Skinned constraints limit deformation around the skeleton position
 *
 * Usage:
 * @code
 * VehicleBuilder<4> builder;
 * builder.SetChassisMesh(mesh)
 *        .SetSoftBodyChassis(SoftBodyChassisSettings::Realistic())
 *        .Build(core);
 * @endcode
 *
 * @note Requires Object::Component::Mesh and Object::Component::Transform
 * @note The RigidBody component is managed internally - do not add manually
 */
struct SoftBodyChassis {
    /// Settings controlling deformation behavior
    SoftBodyChassisSettings settings;

    /// Whether the soft body was successfully created
    /// If false, the system fell back to RigidBody
    bool isActive = false;

    /**
     * @brief Default constructor with default settings
     */
    SoftBodyChassis() : settings(SoftBodyChassisSettings::Default()) {}

    /**
     * @brief Constructor with custom settings
     * @param chassisSettings Settings for the soft body chassis
     */
    explicit SoftBodyChassis(const SoftBodyChassisSettings &chassisSettings)
        : settings(chassisSettings)
    {
    }

    /**
     * @brief Constructor with factory preset
     * @param preset One of the factory presets (Realistic, Arcade, Performance)
     */
    explicit SoftBodyChassis(SoftBodyChassisSettings &&preset) : settings(std::move(preset)) {}
};

} // namespace Physics::Component
