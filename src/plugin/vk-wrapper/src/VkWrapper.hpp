/**************************************************************************
 * VkWrapper v0.0.0
 *
 * VkWrapper is a software package, part of the Engine².
 *
 * This file is part of the VkWrapper project that is under GPL-3.0 License.
 * Copyright © 2024 by @EngineSquared, All rights reserved.
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
 * @example "Usage of the VkWrapper class:"
 * @code
 * ES::Plugin::Window::Resource::Window window(800, 600, "My Engine");
 *
 * ES::Plugin::VkWrapper vkWrapper;
 * ES::Plugin::VkWrapper::CreateInfo createInfo = {
 *     .window = window.GetGLFWWindow(),
 *     .width = 800,
 *     .height = 600,
 *     .applicationName = "My Engine",
 *     .shaders = {{SHADER_DIR "vert.spv", "main"}, {SHADER_DIR "frag.spv", "main"}}
 * };
 * vkWrapper.Create(createInfo);
 *
 * window.SetFramebufferSizeCallback((void *) &vkWrapper, ES::Plugin::VkWrapper::ResizeCallback);
 *
 * vkWrapper.PrintAvailableExtensions();
 *
 * while (!window.ShouldClose())
 * {
 *     glfwPollEvents();
 *     auto result = vkWrapper.DrawFrame();
 *     if (result == ES::Plugin::Wrapper::Result::NeedResize)
 *         vkWrapper.Resize(window.GetGLFWWindow());
 * }
 *
 * vkWrapper.Destroy();
 * @endcode
 */
class VkWrapper {
  public:
    /**
     * @brief Structure to hold the creation information for the Vulkan wrapper.
     *
     * This structure contains all the necessary information required to initialize
     * the Vulkan wrapper, including the window handle, dimensions, application name,
     * and shader paths.
     *
     * @param window  The GLFW window to create the VkWrapper for.
     * @param width  The width of the window.
     * @param height  The height of the window.
     * @param applicationName  The name of the application.
     * @param shaders  The paths to the shaders.
     *
     * @see Wrapper::ShaderModule::ShaderPaths
     */
    struct CreateInfo {
        GLFWwindow *window;
        uint32_t width;
        uint32_t height;
        std::string applicationName;
        Wrapper::ShaderModule::ShaderPaths shaders;
    };

  public:
    /**
     * @brief Create the VkWrapper using the Vulkan API.
     *
     * This function creates the VkWrapper using the Vulkan API. It creates the
     * instance, the surface, the physical device, the logical device, the swap
     * chain, the image views, the render pass, the graphics pipeline, the frame
     * buffers, the command pool, the command buffers, the semaphores, and the fences.
     *
     * @param info  The creation information required for the VkWrapper.
     */
    void Create(const CreateInfo &info);

    /**
     * @brief Destroy the VkWrapper using the Vulkan API.
     *
     * This function destroys the VkWrapper using the Vulkan API. It destroys the
     * fences, the semaphores, the command buffers, the command pool, the frame
     * buffers, the graphics pipeline, the render pass, the image views, the swap
     * chain, the logical device, the physical device, the surface, and the instance.
     */
    void Destroy();

    /**
     * @brief Draw a frame using the Vulkan API.
     *
     * This function draws a frame using the Vulkan API. It waits for the fences
     * to be signaled, resets the fences, acquires the next image in the swap chain,
     * records the command buffer, submits the command buffer to the graphics queue,
     * presents the image to the screen, and increments the current frame index.
     *
     * @return Wrapper::Result  The result of the draw operation. Success if the
     * image was drawn successfully, NeedResize if a Resize is needed.
     *
     * @see Wrapper::Result
     * @see Resize
     */
    [[nodiscard]] Wrapper::Result DrawFrame();

    /**
     * @brief Resize the window using the Vulkan API.
     *
     * This function resizes the window using the Vulkan API. It gets the new
     * width and height of the window, waits for the window to be resized, and
     * recreates the swap chain with the new dimensions.
     *
     * @param window  The GLFW window to resize.
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
    inline void SetFramebufferResized() { _instance.SetFramebufferResized(true); }

    /**
     * @brief Callback function for the framebuffer resize event.
     *
     * This function is called when the framebuffer is resized.
     * It sets the framebuffer resized flag to indicate that the framebuffer
     * needs to be resized.
     *
     * @param window  The GLFW window that was resized.
     * @param width  The new width of the window. (unused)
     * @param height  The new height of the window. (unused)
     */
    static void ResizeCallback(GLFWwindow *window, int width, int height);

    /**
     * @brief Print the available extensions for the Vulkan API.
     *
     */
    void PrintAvailableExtensions() const;

    /**
     * @brief Print the version of the VkWrapper.
     *
     */
    void PrintVersion() const;

    /**
     * @brief Print the configuration of the VkWrapper.
     *
     */
    void PrintConfig() const;

  private:
    Wrapper::Instance _instance;
};

} // namespace ES::Plugin

#endif /* !VKWRAPPER_HPP_ */
