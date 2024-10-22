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
#include <vector>

#include "DebugMessenger.hpp"
#include "PhysicalDevice.hpp"

namespace ES::Plugin::Wrapper {

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

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
     * @brief Picks a suitable physical device (GPU) for Vulkan operations.
     *
     * This function selects a physical device that meets the requirements
     * for running Vulkan applications. It evaluates available GPUs and
     * chooses the most appropriate one based on criteria such as support
     * for required features and extensions.
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
};

} // namespace ES::Plugin::Wrapper

#endif /* !INSTANCE_HPP_ */
