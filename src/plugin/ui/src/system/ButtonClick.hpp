#pragma once

#include "Engine.hpp"

namespace ES::Plugin::UI::System {
/**
 * @brief System to handle a button click event
 * It will call the OnClick event of the button entity
 *
 * @param   core   The registry to use
 */
void ButtonClick(ES::Engine::Core &core);
} // namespace ES::Plugin::UI::System
