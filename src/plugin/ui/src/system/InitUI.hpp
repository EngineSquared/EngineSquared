#pragma once

#include "Engine.hpp"

namespace ES::Plugin::UI::System {
/**
 * @brief Initialize the UI plugin
 *
 * @param core A reference to the core engine, used for the contact listener.
 *
 * @return void
 */
void Init(ES::Engine::Core &core);
void Destroy(ES::Engine::Core &core);
} // namespace ES::Plugin::UI::System
