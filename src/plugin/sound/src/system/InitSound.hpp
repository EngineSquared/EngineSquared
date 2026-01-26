#pragma once

#include "Engine.hpp"

namespace Sound::System {
/**
 * @brief Initialize the sound plugin
 *
 * @param core A reference to the core engine, used for the contact listener.
 *
 * @return void
 */
void InitSounds(Engine::Core &core);

/**
 * @brief Report any audio errors that occurred during callbacks
 *
 * @param core A reference to the core engine.
 *
 * @return void
 */
void ReportAudioErrors(Engine::Core &core);

} // namespace Sound::System
