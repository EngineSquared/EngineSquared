#pragma once

#include "core/Core.hpp"

namespace ES::Plugin::UI::System {
/**
 * @brief Update the mouse position to the UI context
 *
 * @param core A reference to the core engine, used for the contact listener.
 *
 * @return void
 */
void UpdateMouseMoveEvent(ES::Engine::Core &core);
} // namespace ES::Plugin::UI::System
