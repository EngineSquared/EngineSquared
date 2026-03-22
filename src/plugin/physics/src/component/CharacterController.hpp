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
 * @file CharacterController.hpp
 * @brief Public CharacterController component wrapping Jolt's CharacterVirtual
 *
 * This component provides character-specific physics (gravity response,
 * step-up, slope limits) as an alternative to using a raw RigidBody for
 * player entities.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-03-22
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>

namespace Physics::Component {

/**
 * @brief Public CharacterController component
 *
 * Wraps Jolt's JPH::CharacterVirtual to provide character-specific physics
 * behavior including gravity response, automatic step-up over obstacles,
 * and configurable slope angle limits.
 *
 * Unlike RigidBody, a CharacterController does not create a physics body.
 * It uses Jolt's virtual character system which performs shape casts each
 * frame for collision detection without participating in the rigid body
 * simulation. This makes it ideal for player or NPC entities that need
 * precise movement control.
 *
 * The shape used for collision is taken from a CapsuleCollider on the same
 * entity. If no CapsuleCollider is present, a default capsule (halfHeight=0.5,
 * radius=0.3) is used.
 *
 * The user drives movement by writing to @c linearVelocity each frame
 * (e.g., from input or AI code). The system applies gravity and resolves
 * collisions automatically.
 *
 * @note The entity must have an Object::Component::Transform. If absent, a
 *       default Transform is created automatically.
 * @note Do NOT combine CharacterController with RigidBody on the same entity.
 *
 * @example Basic player setup
 * @code
 * auto player = core.CreateEntity();
 * player.AddComponent<Object::Component::Transform>(glm::vec3(0.0f, 5.0f, 0.0f));
 * player.AddComponent<Physics::Component::CapsuleCollider>(0.8f, 0.3f);
 * player.AddComponent<Physics::Component::CharacterController>(
 *     Physics::Component::CharacterController::Create(70.0f));
 * @endcode
 *
 * @example Applying movement each frame
 * @code
 * auto &cc = player.GetComponents<Physics::Component::CharacterController>();
 * cc.linearVelocity = glm::vec3(inputX * speed, cc.linearVelocity.y, inputZ * speed);
 * @endcode
 */
struct CharacterController {
    //========================================================================
    // Physics Settings
    //========================================================================

    /// Maximum walkable slope angle in degrees (default 50°)
    float maxSlopeAngle = 50.0f;

    /// Maximum height (in metres) the character can step up over (default 0.3 m)
    float maxStepHeight = 0.3f;

    /// Gravity scale factor (1.0 = normal gravity, 0.0 = no gravity)
    float gravityFactor = 1.0f;

    /// Character mass in kg (affects interactions with dynamic bodies)
    float mass = 70.0f;

    //========================================================================
    // Runtime State (written by user each frame to drive movement)
    //========================================================================

    /// Desired linear velocity in world space (m/s).
    /// Set this every frame from input or AI. The Y component is modified by
    /// gravity automatically; override it only if you manage gravity yourself
    /// (set gravityFactor = 0).
    glm::vec3 linearVelocity{0.0f, 0.0f, 0.0f};

    //========================================================================
    // Helper Factory Methods
    //========================================================================

    /**
     * @brief Create a CharacterController with default settings
     * @param bodyMass Mass of the character in kg (default 70 kg)
     * @return CharacterController with sensible defaults
     */
    static CharacterController Create(float bodyMass = 70.0f)
    {
        CharacterController cc;
        cc.mass = bodyMass;
        return cc;
    }
};

} // namespace Physics::Component
