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

/**
 * @brief Bind the Event callback to support inputs on the UI
 *
 * @param core A reference to the core engine, used for the contact listener.
 *
 * @return void
 */
void BindEventCallback(ES::Engine::Core &core);

/**
 * @brief Destroy the UI plugin
 *
 * @param core A reference to the core engine, used for the contact listener.
 *
 * @return void
 */
void Destroy(ES::Engine::Core &core);
} // namespace ES::Plugin::UI::System
