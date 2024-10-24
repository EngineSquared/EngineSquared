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

#include <cstdlib>
#include <cstring>

#include "GraphicsPipeline.hpp"
#include "ImageView.hpp"
#include "LogicalDevice.hpp"
#include "PhysicalDevice.hpp"
#include "Surface.hpp"
#include "RenderPass.hpp"

namespace ES::Plugin::Wrapper {

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
 * Instance instance("My Engine");
 * @endcode
 */
class Instance {
  public:
    /**
     * @brief Constructor for the Instance class.
     *
     * Initializes the Vulkan instance with the specified application name.
     *
     * @param applicationName The name of the application.
     */
    Instance(const std::string &applicationName);

    /**
     * @brief Destructor for the Instance class.
     *
     * Cleans up and destroys the Vulkan instance and related resources.
     */
    ~Instance();

    /**
     * @brief Sets up the debug messenger for Vulkan instance.
     *
     * This function initializes and configures the debug messenger
     * which is used to capture and handle debug messages from the Vulkan
     * API. It is essential for debugging and validation purposes, providing
     * detailed information about the Vulkan operations and potential issues.
     */
    void setupDebugMessenger();

    /**
     * @brief Creates a Vulkan surface for the given GLFW window.
     *
     * This function initializes a Vulkan surface that is associated with the specified
     * GLFW window. The surface is necessary for rendering Vulkan graphics to the window.
     *
     * @param window A pointer to the GLFWwindow for which the Vulkan surface will be created.
     */
    void createSurface(GLFWwindow *window);

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
    void setupDevices();

    /**
     * @brief Creates swap chain images for the Vulkan API.
     *
     * This function creates a swap chain for the Vulkan API. The swap chain
     * is used to manage the presentation of images to the screen. It is
     * essential for rendering graphics to the window and ensuring smooth
     * frame transitions.
     *
     * @param width The width of the swap chain.
     * @param height The height of the swap chain.
     */
    void createSwapChainImages(const uint32_t width, const uint32_t height);

    void createGraphicsPipeline();

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
    [[nodiscard]] bool CheckValidationLayerSupport();

    /**
     * @brief Gets the required extensions for the Vulkan API.
     *
     * This function retrieves the list of required extensions that need to be
     * enabled for the Vulkan instance.
     *
     * @return std::vector<const char *> The required extensions for the Vulkan API.
     */
    [[nodiscard]] std::vector<const char *> getRequiredExtensions();

  private:
    VkInstance _instance;
    DebugMessenger _debugMessenger;
    PhysicalDevice _physicalDevice;
    LogicalDevice _logicalDevice;
    Surface _surface;
    SwapChain _swapChain;
    ImageView _imageView;
    GraphicsPipeline _graphicsPipeline;
    RenderPass _renderPass;
};

} // namespace ES::Plugin::Wrapper

#endif /* !INSTANCE_HPP_ */
