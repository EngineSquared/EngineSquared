#pragma once

#include "Engine.hpp"

namespace ES::Plugin::UI::System {
/**
 * @brief System to update the state of a button
 *
 * @param   core   The registry to use
 * @param   e   The entity to update
 */
void UpdateButtonState(ES::Engine::Core &core);
} // namespace ES::Plugin::UI::System
