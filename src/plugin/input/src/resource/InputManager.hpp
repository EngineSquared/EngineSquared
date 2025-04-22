#pragma once

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

#include "Core.hpp"

namespace ES::Plugin::Input::Resource {
/**
 * InputManager is a singleton class that wraps the GLFW callback functions.
 */
class InputManager {
  public:
    InputManager() = default;
    ~InputManager() = default;
};
} // namespace ES::Plugin::Input::Resource
