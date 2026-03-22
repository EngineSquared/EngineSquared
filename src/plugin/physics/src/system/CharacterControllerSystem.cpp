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
 * @file CharacterControllerSystem.cpp
 * @brief CharacterController lifecycle and per-frame update implementation
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-03-22
 **************************************************************************/

#include "Physics.pch.hpp"

#include "system/CharacterControllerSystem.hpp"

#include "Logger.hpp"
#include "component/CapsuleCollider.hpp"
#include "component/CharacterController.hpp"
#include "component/CharacterControllerInternal.hpp"
#include "exception/CharacterControllerError.hpp"
#include "resource/PhysicsManager.hpp"
#include "scheduler/FixedTimeUpdate.hpp"
#include "utils/JoltConversions.hpp"

#include "Object.hpp"

#include <Jolt/Physics/Character/CharacterVirtual.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>

#include <fmt/format.h>

namespace Physics::System {

//=============================================================================
// Entt hook callbacks
//=============================================================================

/**
 * @brief Called when CharacterController is added to an entity.
 *
 * Builds a JPH::CharacterVirtual from the component settings and from
 * the CapsuleCollider (if present), then stores it in
 * CharacterControllerInternal.
 */
static void OnCharacterControllerConstruct(Engine::Core::Registry &registry, Engine::EntityId entityId)
{
    try
    {
        auto &core = registry.ctx().get<Engine::Core *>();
        Engine::Entity entity{*core, entityId};

        auto &physicsManager = core->GetResource<Resource::PhysicsManager>();
        if (!physicsManager.IsPhysicsActivated())
        {
            Log::Error("Cannot create CharacterController: Physics system not activated");
            return;
        }

        auto &cc = entity.GetComponents<Component::CharacterController>();

        auto *transform = entity.TryGetComponent<Object::Component::Transform>();
        if (!transform)
        {
            Log::Warn("CharacterController added to entity without Transform - creating default Transform");
            transform = &entity.AddComponent<Object::Component::Transform>();
        }

        // Build collision shape: prefer a CapsuleCollider on the entity,
        // fall back to a sensible default (0.5 m half-height, 0.3 m radius).
        JPH::RefConst<JPH::Shape> shape;
        if (auto *capsule = entity.TryGetComponent<Component::CapsuleCollider>())
        {
            if (capsule->IsValid())
            {
                shape = new JPH::CapsuleShape(capsule->halfHeight, capsule->radius);
            }
            else
            {
                Log::Warn("CharacterController: CapsuleCollider is invalid, using default capsule shape");
                shape = new JPH::CapsuleShape(0.5f, 0.3f);
            }
        }
        else
        {
            shape = new JPH::CapsuleShape(0.5f, 0.3f);
        }

        JPH::CharacterVirtualSettings settings;
        settings.mMaxSlopeAngle = JPH::DegreesToRadians(cc.maxSlopeAngle);
        settings.mShape = shape;
        settings.mMass = cc.mass;

        auto *character = new JPH::CharacterVirtual(&settings, Utils::ToJoltRVec3(transform->GetPosition()),
                                                    Utils::ToJoltQuat(transform->GetRotation()),
                                                    static_cast<JPH::uint64>(entityId),
                                                    &physicsManager.GetPhysicsSystem());

        entity.AddComponent<Component::CharacterControllerInternal>(character);

        Log::Debug(fmt::format("Created CharacterController for entity {}", entity));
    }
    catch (const Exception::CharacterControllerError &e)
    {
        Log::Error(fmt::format("CharacterControllerError in OnCharacterControllerConstruct: {}", e.what()));
    }
}

/**
 * @brief Called when CharacterController is removed from an entity.
 *
 * Releases the Jolt CharacterVirtual and removes CharacterControllerInternal.
 */
static void OnCharacterControllerDestroy(Engine::Core::Registry &registry, Engine::EntityId entityId)
{
    try
    {
        auto &core = registry.ctx().get<Engine::Core *>();
        Engine::Entity entity{*core, entityId};

        auto *internal = entity.TryGetComponent<Component::CharacterControllerInternal>();
        if (!internal || !internal->IsValid())
            return;

        internal->character = nullptr;

        Log::Debug(fmt::format("Destroyed CharacterController for entity {}", entity));

        entity.RemoveComponent<Component::CharacterControllerInternal>();
    }
    catch (const Exception::CharacterControllerError &e)
    {
        Log::Error(fmt::format("CharacterControllerError in OnCharacterControllerDestroy: {}", e.what()));
    }
}

//=============================================================================
// Per-frame update
//=============================================================================

void CharacterControllerUpdate(Engine::Core &core)
{
    auto &physicsManager = core.GetResource<Resource::PhysicsManager>();
    if (!physicsManager.IsPhysicsActivated())
        return;

    auto dt = core.GetScheduler<Engine::Scheduler::FixedTimeUpdate>().GetTickRate();
    auto &registry = core.GetRegistry();

    auto view = registry.view<Component::CharacterController, Component::CharacterControllerInternal,
                              Object::Component::Transform>();

    for (auto entityId : view)
    {
        auto &cc = view.get<Component::CharacterController>(entityId);
        auto &internal = view.get<Component::CharacterControllerInternal>(entityId);
        auto &transform = view.get<Object::Component::Transform>(entityId);

        if (!internal.IsValid())
            continue;

        auto &character = *internal.character;

        // Gravity in world space scaled by this character's factor.
        JPH::Vec3 gravity = physicsManager.GetPhysicsSystem().GetGravity() * cc.gravityFactor;

        // Build the velocity to apply this frame.
        // The CharacterVirtual::Update comment states: "It is the application's
        // responsibility to apply gravity to the character velocity."
        // So we accumulate gravity into the vertical component ourselves:
        // - When airborne  : add gravity * dt to keep falling.
        // - When supported : clamp downward velocity to 0 (character rests on floor).
        //   This lets the user set a positive Y (jump impulse) that survives until the
        //   character leaves the ground, while preventing gravity from piling up
        //   across frames once we've landed.
        JPH::Vec3 velocity = Utils::ToJoltVec3(cc.linearVelocity);

        if (character.IsSupported())
        {
            // On surface – don't accumulate downward velocity, but preserve any
            // upward impulse the user may have set this frame (e.g. for jumping).
            if (velocity.GetY() < 0.0f)
                velocity.SetY(0.0f);
        }
        else
        {
            // Airborne – accumulate gravity into the vertical component.
            velocity.SetY(velocity.GetY() + gravity.GetY() * dt);
        }

        character.SetLinearVelocity(velocity);

        // Extended update handles stair-stepping via mWalkStairsStepUp /
        // mStickToFloorStepDown so the character can climb steps up to
        // maxStepHeight metres tall.
        JPH::CharacterVirtual::ExtendedUpdateSettings extendedSettings;
        extendedSettings.mStickToFloorStepDown = JPH::Vec3(0.0f, -cc.maxStepHeight, 0.0f);
        extendedSettings.mWalkStairsStepUp = JPH::Vec3(0.0f, cc.maxStepHeight, 0.0f);

        // Use default filters: the CharacterVirtual will collide with all
        // broad-phase layers and object layers.
        JPH::BroadPhaseLayerFilter broadPhaseFilter;
        JPH::ObjectLayerFilter objectLayerFilter;
        JPH::BodyFilter bodyFilter;
        JPH::ShapeFilter shapeFilter;

        character.ExtendedUpdate(dt, gravity, extendedSettings, broadPhaseFilter, objectLayerFilter, bodyFilter,
                                 shapeFilter, *physicsManager.GetTempAllocator());

        // Write back the resulting velocity so that:
        //   1) The user can read the current speed (including accumulated gravity).
        //   2) The accumulated Y velocity is preserved across frames without extra state.
        cc.linearVelocity = Utils::FromJoltVec3(character.GetLinearVelocity());

        // Sync Jolt position/rotation back to the entity Transform.
        transform.SetPosition(Utils::FromJoltRVec3(character.GetPosition()));
        transform.SetRotation(Utils::FromJoltQuat(character.GetRotation()));
    }
}

//=============================================================================
// Public system functions
//=============================================================================

void InitCharacterControllerSystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    if (!registry.ctx().contains<Engine::Core *>())
        registry.ctx().emplace<Engine::Core *>(&core);

    registry.on_construct<Component::CharacterController>().connect<&OnCharacterControllerConstruct>();
    registry.on_destroy<Component::CharacterController>().connect<&OnCharacterControllerDestroy>();
}

void ShutdownCharacterControllerSystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    registry.on_construct<Component::CharacterController>().disconnect<&OnCharacterControllerConstruct>();
    registry.on_destroy<Component::CharacterController>().disconnect<&OnCharacterControllerDestroy>();
}

} // namespace Physics::System
