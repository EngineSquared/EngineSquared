#pragma once

#include "Engine.hpp"

namespace ES::Plugin::Input::System {
/**
 * @brief Bind GLFW callbacks to the InputManager.
 *
 * This function binds the GLFW callbacks to the InputManager.
 * It should be called after the window is created.
 *
 * @param core The Engine² Core.
 */
void BindCallbacksToGLFW(ES::Engine::Core &core);
} // namespace ES::Plugin::Input::System
