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
 * @file CharacterControllerInternal.hpp
 * @brief Internal component storing Jolt CharacterVirtual state
 *
 * This component is managed automatically by CharacterControllerSystem
 * via entt hooks. Users should never interact with it directly.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-03-22
 **************************************************************************/

#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/Character/CharacterVirtual.h>

namespace Physics::Component {

/**
 * @brief Internal component that holds the Jolt CharacterVirtual object.
 *
 * Managed automatically by CharacterControllerSystem. Do not add or remove
 * this component manually.
 *
 * @warning Internal use only! Do not use directly.
 */
struct CharacterControllerInternal {
    /// Jolt virtual character (reference-counted, null until constructed)
    JPH::Ref<JPH::CharacterVirtual> character;

    /**
     * @brief Default constructor (no character yet)
     */
    CharacterControllerInternal() = default;

    /**
     * @brief Construct with an existing CharacterVirtual
     * @param inCharacter The Jolt CharacterVirtual to wrap
     */
    explicit CharacterControllerInternal(JPH::CharacterVirtual *inCharacter) : character(inCharacter) {}

    /**
     * @brief Check whether the internal character is valid
     */
    [[nodiscard]] bool IsValid() const { return character != nullptr; }
};

} // namespace Physics::Component
