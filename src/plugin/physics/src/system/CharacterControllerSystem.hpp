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
 * @file CharacterControllerSystem.hpp
 * @brief System managing CharacterController lifecycle and per-frame update
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-03-22
 **************************************************************************/

#pragma once

#include "core/Core.hpp"

namespace Physics::System {

/**
 * @brief Initialize CharacterController system and register entt hooks.
 *
 * Registers on_construct / on_destroy hooks for
 * Physics::Component::CharacterController so that the Jolt
 * CharacterVirtual object is automatically created/destroyed when the
 * component is added to or removed from an entity.
 *
 * @param core The engine core
 * @note Use with the Startup scheduler.
 */
void InitCharacterControllerSystem(Engine::Core &core);

/**
 * @brief Shut down the CharacterController system and unregister hooks.
 *
 * @param core The engine core
 * @note Use with the Shutdown scheduler.
 */
void ShutdownCharacterControllerSystem(Engine::Core &core);

/**
 * @brief Per-frame update for all active CharacterControllers.
 *
 * For every entity with CharacterController + CharacterControllerInternal
 * + Transform this system:
 *   1. Applies the component's @c linearVelocity to the Jolt character.
 *   2. Calls JPH::CharacterVirtual::ExtendedUpdate to step the character,
 *      resolve collisions, apply gravity and handle step-up logic.
 *   3. Writes the updated position / rotation back to the Transform.
 *
 * @param core The engine core
 * @note Use with the FixedTimeUpdate scheduler, after PhysicsUpdate.
 */
void CharacterControllerUpdate(Engine::Core &core);

} // namespace Physics::System
