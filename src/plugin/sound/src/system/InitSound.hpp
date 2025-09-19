#pragma once

#include "Engine.hpp"

namespace Plugin::Sound::System {
/**
 * @brief Initialize the sound plugin
 *
 * @param core A reference to the core engine, used for the contact listener.
 *
 * @return void
 */
void InitSounds(Engine::Core &core);
} // namespace Plugin::Sound::System
