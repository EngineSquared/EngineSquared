#pragma once

#include "core/Core.hpp"

namespace Physics::System {

void InitCharacterControllerSystem(Engine::Core &core);

void ShutdownCharacterControllerSystem(Engine::Core &core);

void CharacterControllerUpdate(Engine::Core &core);

} // namespace Physics::System
