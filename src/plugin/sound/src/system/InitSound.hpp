#pragma once

#include "Engine.hpp"

namespace ES::Plugin::Sound::System {
/**
 * @brief Initialize the sound plugin
 *
 * @param core A reference to the core engine, used for the contact listener.
 *
 * @return void
 */
void InitSounds(ES::Engine::Core &core);
} // namespace ES::Plugin::Sound::System
