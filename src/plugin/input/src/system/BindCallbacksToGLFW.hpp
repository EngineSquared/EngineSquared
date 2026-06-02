#pragma once

#include "Engine.hpp"

namespace Input::System {
/**
 * @brief Attach GLFW callbacks to the InputManager.
 *
 * This function binds the GLFW callbacks to the InputManager.
 * It should be called after the window is created.
 *
 * @param core The Engine² Core.
 */
void BindCallbacksToGLFW(Engine::Core &core);
} // namespace Input::System
