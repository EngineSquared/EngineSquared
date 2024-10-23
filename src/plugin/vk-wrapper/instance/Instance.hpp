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

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <cstdlib>
#include <cstring>
#include <string>

#include "LogicalDevice.hpp"
#include "PhysicalDevice.hpp"
#include "Surface.hpp"

namespace ES::Plugin::Wrapper {

/**
 * @brief Instance class.
 *
 * This class is used to create an instance of the Vulkan API.
 *
 * @example
 * @code
 * Instance instance("My Engine");
 * @endcode
 */
class Instance {
  public:
    Instance(const std::string &applicationName);
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
     * @brief Picks a suitable physical device (GPU) for Vulkan operations.
     *
     * This function selects a physical device that meets the requirements
     * for running Vulkan applications. It evaluates available GPUs and
     * chooses the most appropriate one based on criteria such as support
     * for required features and extensions.
     *
     * @brief Creates a logical device from the selected physical device.
     *
     * This function creates a logical device, which is an abstraction
     * representing the GPU. It enables communication with the physical
     * device and allows the application to execute Vulkan commands.
     * The logical device is configured with specific features and
     * extensions required by the application.
     */
    void setupDevices();

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
     * @brief Get the Required Extensions object
     *
     * @return std::vector<const char *>  The required extensions for the Vulkan API.
     */
    [[nodiscard]] std::vector<const char *> getRequiredExtensions();

  private:
    VkInstance _instance;
    DebugMessenger _debugMessenger;
    PhysicalDevice _physicalDevice;
    LogicalDevice _logicalDevice;
    Surface _surface;
};

} // namespace ES::Plugin::Wrapper

#endif /* !INSTANCE_HPP_ */
