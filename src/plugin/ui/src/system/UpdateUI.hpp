#pragma once

#include "Engine.hpp"

namespace ES::Plugin::UI::System {
/**
 * @brief Run and update the UI
 *
 * @param core A reference to the core engine, used for the contact listener.
 *
 * @return void
 */
void Update(ES::Engine::Core &core);

/**
 * @brief Render the loaded document
 *
 * @param core A reference to the core engine, used for the contact listener.
 *
 * @return void
 */
void Render(ES::Engine::Core &core);
} // namespace ES::Plugin::UI::System
