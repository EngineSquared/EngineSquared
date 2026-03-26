#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/Character/CharacterVirtual.h>

namespace Physics::Component {

/// @brief Internal component that holds the Jolt CharacterVirtual object.
///     Managed automatically by the plugin.
/// @warning Internal use only! Do not use directly.
struct CharacterControllerInternal {
    /// @brief Jolt virtual character (reference-counted, null until constructed)
    JPH::Ref<JPH::CharacterVirtual> character;

    /// @brief Default constructor
    CharacterControllerInternal() = default;

    /// @brief Construct with an existing CharacterVirtual
    /// @param inCharacter The Jolt CharacterVirtual to wrap
    explicit CharacterControllerInternal(JPH::CharacterVirtual *inCharacter) : character(inCharacter) {}

    /// @brief Check whether the internal character is valid
    [[nodiscard]] bool IsValid() const { return character != nullptr; }
};

} // namespace Physics::Component
