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
 * @file Instance.hpp
 * @brief Instance class declaration.
 *
 * This class is used to create an instance of the Vulkan API.
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-15
 **************************************************************************/

#ifndef INSTANCE_HPP_
#define INSTANCE_HPP_

#include <algorithm>
#include <cstdlib>
#include <ranges>

#include "Command.hpp"
#include "Descriptor.hpp"
#include "Framebuffer.hpp"
#include "GraphicsPipeline.hpp"
#include "LogicalDevice.hpp"
#include "PhysicalDevice.hpp"
#include "RenderPass.hpp"
#include "Surface.hpp"

namespace ES::Plugin::Wrapper {

/**
 * @brief Result enum class.
 *
 * This enum class is used to represent the result of an operation.
 * It can be either Success or NeedResize.
 * It is used to indicate the success or if a resize is needed.
 */
enum class Result : bool {
    Success = true,
    NeedResize = false,
};

/**
 * @class Instance
 * @brief Manages the Vulkan instance and related resources.
 *
 * The Instance class is responsible for initializing and managing the Vulkan
 * instance, setting up the debug messenger, creating surfaces, selecting
 * physical devices, and creating logical devices and swap chains. It provides
 * essential functionality for setting up and managing Vulkan operations.
 *
 * @example
 * @code
 * Instance instance;
 * instance.Create("My Engine");
 *
 * instance.SetupDebugMessenger();
 * instance.CreateSurface(window);
 * instance.SetupDevices();
 * instance.CreateSwapChainImages(width, height);
 * instance.CreateGraphicsPipeline(shaders);
 * instance.CreateSyncObjects();
 *
 * while (!glfwWindowShouldClose(window))
 * {
 *    if (instance.DrawNextImage() == Result::NeedResize)
 *        instance.RecreateSwapChain(width, height);
 * }
 *
 * instance.Destroy();
 * @endcode
 */
class Instance {
  public:
    /**
     * @brief Constructor for the Instance class.
     *
     * Initializes the Vulkan instance with the specified application name.
     *
     * @param applicationName  The name of the application.
     */
    void Create(const std::string &applicationName);

    /**
     * @brief Destructor for the Instance class.
     *
     * Cleans up and destroys the Vulkan instance and related resources.
     */
    void Destroy(entt::resource_cache<Texture, TextureLoader> &textures);

    /**
     * @brief Sets up the debug messenger for Vulkan instance.
     *
     * This function initializes and configures the debug messenger
     * which is used to capture and handle debug messages from the Vulkan
     * API. It is essential for debugging and validation purposes, providing
     * detailed information about the Vulkan operations and potential issues.
     */
    void SetupDebugMessenger();

    /**
     * @brief Creates a Vulkan surface for the given GLFW window.
     *
     * This function initializes a Vulkan surface that is associated with the specified
     * GLFW window. The surface is necessary for rendering Vulkan graphics to the window.
     *
     * @param window  A pointer to the GLFWwindow for which the Vulkan surface will be created.
     */
    void CreateSurface(GLFWwindow *window);

    /**
     * @brief Picks a suitable physical device (GPU) for Vulkan operations and creates a logical device.
     *
     * This function selects a physical device that meets the requirements
     * for running Vulkan applications. It evaluates available GPUs and
     * chooses the most appropriate one based on criteria such as support
     * for required features and extensions. It then creates a logical device,
     * which is an abstraction representing the GPU. The logical device enables
     * communication with the physical device and allows the application to
     * execute Vulkan commands. It is configured with specific features and
     * extensions required by the application.
     */
    void SetupDevices();

    /**
     * @brief Creates swap chain images for the Vulkan API.
     *
     * This function creates a swap chain for the Vulkan API. The swap chain
     * is used to manage the presentation of images to the screen. It is
     * essential for rendering graphics to the window and ensuring smooth
     * frame transitions.
     *
     * @param window  The GLFW window for which the swap chain images will be created.
     */
    void CreateSwapChainImages(const uint32_t width, const uint32_t height);

    /**
     * @brief Create a Graphics Pipeline object.
     *
     * This function creates a graphics pipeline for the Vulkan API. The graphics
     * pipeline is used to render graphics to the screen. It defines the sequence
     * of operations that are performed on the input data to produce the final
     * rendered image. The graphics pipeline includes a series of stages such as
     * vertex processing, tessellation, geometry shading, fragment shading, and
     * blending.
     * The graphics pipeline also includes a render pass, which defines the
     * attachments used for rendering and the subpasses that are executed during
     * rendering.
     *
     * @param shaders  The paths to the shader files.
     * @param textures  The textures used in the graphics pipeline.
     * @param models  The models used in the graphics pipeline.
     */
    void
    CreateGraphicsPipeline(const ShaderModule::ShaderPaths &shaders,
                           const entt::resource_cache<Texture, TextureLoader> &textures,
                           const entt::resource_cache<Object::Component::Mesh, Object::Component::MeshLoader> &models);

    /**
     * @brief Create synchronization objects for the Vulkan API.
     *
     * This function creates synchronization objects for the Vulkan API. Synchronization
     * objects are used to coordinate the execution of commands and ensure that
     * operations are performed in the correct order. They are essential for
     * managing the rendering process and avoiding synchronization issues.
     */
    void CreateSyncObjects();

    /**
     * @brief Recreates the swap chain for the Vulkan API.
     *
     * This function recreates the swap chain for the Vulkan API. It is called
     * when the window is resized or when the swap chain needs to be recreated
     * for any other reason. The function waits for the device to become idle,
     * cleans up the existing swap chain, and creates a new swap chain with the
     * updated dimensions.
     */
    void RecreateSwapChain(const uint32_t width, const uint32_t height);

    /**
     * @brief Draws the next image in the swap chain.
     *
     * This function draws the next image in the swap chain for the Vulkan API.
     * It waits for the fences to be signaled, resets the fences, acquires the
     * next image in the swap chain, records the command buffer, and submits the
     * command buffer to the graphics queue. It then presents the image to the
     * screen and increments the current frame index.
     *
     * @return Result The result of the draw operation. Success if the image was
     * drawn successfully, NeedResize if the swap chain needs to be recreated.
     */
    [[nodiscard]] Result DrawNextImage();

    /**
     * @brief Sets the framebuffer resized flag.
     *
     * This function sets the framebuffer resized flag to indicate that the
     * framebuffer needs to be resized.
     * The flag is used to trigger the recreation of the swap chain when the
     * window is resized.
     *
     * @param resized  The new value of the framebuffer resized flag.
     */
    void SetFramebufferResized(bool resized) { _framebufferResized = resized; }

    /**
     * @brief Sets the clear color for the Vulkan API.
     *
     * This function sets the clear color for the Vulkan API. The clear color
     * is used to clear the screen before rendering the next frame. It is
     * specified as a vector of four floating-point values representing the
     * red, green, blue, and alpha components of the color.
     *
     * @param color  The clear color to set.
     */
    void SetClearColor(const glm::vec4 &color) { _command.SetClearColor(color); }

  private:
    /**
     * @brief Checks if the required Vulkan validation layers are supported.
     *
     * This function verifies whether the necessary Vulkan validation layers
     * are available on the system. Validation layers are used for debugging
     * and development purposes to ensure that the Vulkan API is used correctly.
     *
     * @return true if all required validation layers are supported, false otherwise.
     */
    [[nodiscard]] bool CheckValidationLayerSupport() const;

    /**
     * @brief Gets the required extensions for the Vulkan API.
     *
     * This function retrieves the list of required extensions that need to be
     * enabled for the Vulkan instance.
     *
     * @return std::vector<const char *>  The required extensions for the Vulkan API.
     */
    [[nodiscard]] std::vector<const char *> GetRequiredExtensions() const;

    /**
     * @brief Cleans up the swap chain for the Vulkan API.
     *
     * This function cleans up the swap chain for the Vulkan API. It destroys
     * the framebuffers, command buffers, graphics pipeline, render pass, image
     * views, and swap chain. It is called when the swap chain needs to be
     * recreated or when the application is shutting down.
     *
     * @param device  The Vulkan logical device used to clean up the swap chain.
     */
    void CleanupSwapChain(const VkDevice &device);

  private:
    VkInstance _instance;
    DebugMessenger _debugMessenger;
    PhysicalDevice _physicalDevice;
    LogicalDevice _logicalDevice;
    Surface _surface;
    SwapChain _swapChain;
    ImageView _imageView;
    Descriptor _descriptorLayout;
    GraphicsPipeline _graphicsPipeline;
    RenderPass _renderPass;
    Framebuffer _framebuffer;
    Buffers _buffers;
    Command _command;
    std::vector<VkSemaphore> _imageAvailableSemaphores;
    std::vector<VkSemaphore> _renderFinishedSemaphores;
    std::vector<VkFence> _inFlightFences;
    uint32_t _currentFrame = 0;
    bool _framebufferResized = false;
};

} // namespace ES::Plugin::Wrapper

#endif /* !INSTANCE_HPP_ */
