#pragma once

#include "Engine.hpp"

namespace ES::Plugin::UI::System {
/**
 * @brief System to update the state of a button
 *
 * @param   r   The registry to use
 * @param   e   The entity to update
 */
void UpdateButtonState(ES::Engine::Registry &r);
} // namespace ES::Plugin::UI::System
