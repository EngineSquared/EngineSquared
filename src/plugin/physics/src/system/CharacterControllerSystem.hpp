#pragma once

#include "core/Core.hpp"

namespace Physics::System {

/// @brief Initialize the character controller system. This will set up necessary resources and state for character
/// controllers to function.
/// @param core The engine core to initialize the system with.
void InitCharacterControllerSystem(Engine::Core &core);

/// @brief Shutdown the character controller system. This will clean up any resources used by the system.
/// @param core The engine core to shut down the system from.
void ShutdownCharacterControllerSystem(Engine::Core &core);

/// @brief Update the character controller system. This will process all entities with character controllers and
/// update their state based on input and physics interactions.
/// @param core The engine core to update the system with.
void CharacterControllerUpdate(Engine::Core &core);

} // namespace Physics::System
