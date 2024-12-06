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
    VkWrapper(GLFWwindow *window, const std::string &applicationName);
    ~VkWrapper();

    /**
     * @brief Draw a frame using the Vulkan API.
     *
     * This function draws a frame using the Vulkan API. It waits for the fences
     * to be signaled, resets the fences, acquires the next image in the swap chain,
     * records the command buffer, submits the command buffer to the graphics queue,
     * presents the image to the screen, and increments the current frame index.
     *
     * @return Wrapper::Result The result of the draw operation. Success if the
     * image was drawn successfully, Failure if a Resize is needed.
     *
     * @see Wrapper::Result
     * @see Resize
     */
    [[nodiscard]] Wrapper::Result drawFrame();

    /**
     * @brief Resize the window using the Vulkan API.
     *
     * This function resizes the window using the Vulkan API. It gets the new
     * width and height of the window, waits for the window to be resized, and
     * recreates the swap chain with the new dimensions.
     *
     * @param window The GLFW window to resize.
     */
    void Resize(GLFWwindow *window);

    /**
     * @brief Set the framebuffer resized flag.
     *
     * This function sets the framebuffer resized flag to indicate that the
     * framebuffer needs to be resized.
     * The flag is used to trigger the recreation of the swap chain when the
     * window is resized.
     */
    inline void setFramebufferResized() { _instance.setFramebufferResized(true); }

    /**
     * @brief Callback function for the framebuffer resize event.
     *
     * This function is called when the framebuffer is resized.
     * It sets the framebuffer resized flag to indicate that the framebuffer
     * needs to be resized.
     *
     * @param window The GLFW window that was resized.
     * @param width The new width of the window. (unused)
     * @param height The new height of the window. (unused)
     */
    static void ResizeCallback(GLFWwindow *window, int width, int height);

  protected:
  private:
    Wrapper::Instance _instance;
};

} // namespace ES::Plugin

#endif /* !VKWRAPPER_HPP_ */
