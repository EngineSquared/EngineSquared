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
 * vkWrapper.PrintAvailableExtensions();
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
    VkWrapper(GLFWwindow *window, uint32_t width, uint32_t height, const std::string &applicationName);
    ~VkWrapper();

    void drawFrame();

    /**
     * @brief Print the available extensions for the Vulkan API.
     *
     */
    void PrintAvailableExtensions();

  protected:
  private:
    Wrapper::Instance _instance;
};

} // namespace ES::Plugin

#endif /* !VKWRAPPER_HPP_ */
