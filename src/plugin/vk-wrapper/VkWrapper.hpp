/**************************************************************************
 * VkWrapper v0.0.0
 *
 * VkWrapper is a software package, part of the Engine².
 *
 * This file is part of the VkWrapper project that is under GPL-3.0 License.
 * Copyright © 2024 by @MasterLaplace, All rights reserved.
 *
 * VkWrapper is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file VKwrapper.hpp
 * @brief VkWrapper class declaration.
 *
 * This class is a wrapper for the Vulkan API.
 * It is used to simplify the use of Vulkan in the Engine².
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-15
 **************************************************************************/

#ifndef VKWRAPPER_HPP_
#define VKWRAPPER_HPP_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#ifndef UINT32_MAX
#include <limits>
#define UINT32_MAX std::numeric_limits<uint32_t>::max()
#endif

#include "config.h"
#include "export.h"

#include "Instance.hpp"

namespace ES::Plugin {

/**
 * @brief VkWrapper class.
 *
 * This class is a wrapper for the Vulkan API.
 * It is used to simplify the use of Vulkan in the Engine².
 *
 * @example
 * @code
 * // Create a window
 * ES::Plugin::Window::Resource::Window window(800, 600, "My Engine");
 *
 * // Create a VkWrapper
 * ES::Plugin::VkWrapper vkWrapper("My Engine");
 *
 * while (!window.ShouldClose())
 * {
 *     glfwPollEvents();
 *     vkWrapper.DrawFrame();
 * }
 * @endcode
 */
class VkWrapper {
  public:
    VkWrapper(const std::string &applicationName);
    ~VkWrapper();

    void drawFrame();

  protected:
  private:
    Wrapper::Instance _instance;
};

} // namespace ES::Plugin

#endif /* !VKWRAPPER_HPP_ */
