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
 * @file DebugMessenger.hpp
 * @brief DebugMessenger class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-22
 **************************************************************************/

#ifndef DEBUGMESSENGER_HPP_
#define DEBUGMESSENGER_HPP_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "Logger.hpp"
#include "config.h"
#include "export.h"

#include "VkWrapperError.hpp"

namespace ES::Plugin::Wrapper {

const std::vector<const char *> VALIDATION_LAYERS = {"VK_LAYER_KHRONOS_validation"};

#ifdef VKWRAPPER_DEBUG
constexpr bool enableValidationLayers = true;
#else
constexpr bool enableValidationLayers = false;
#endif

/**
 * @brief DebugMessenger class.
 *
 * This class is used to create a debug messenger for the Vulkan API.
 *
 * @example
 * @code
 * DebugMessenger debugMessenger;
 * debugMessenger.SetupDebugMessenger(instance);
 * @endcode
 */
class DebugMessenger {
  public:
    DebugMessenger() = default;
    ~DebugMessenger();

    /**
     * @brief Sets up the debug messenger for the Vulkan instance.
     *
     * This function initializes and configures the debug messenger
     * which is used to capture and handle debug messages from the Vulkan
     * API. It is essential for debugging and validation purposes, providing
     * detailed information about the Vulkan operations and potential issues.
     *
     * @param instance  The Vulkan instance.
     */
    void SetupDebugMessenger(const VkInstance &instance);

    /**
     * @brief Callback function for the debug messenger.
     *
     * This function is called by the Vulkan API when a debug message is
     * generated. It is used to handle the debug messages and display them
     * in the console.
     *
     * @param messageSeverity  The severity of the message.
     * @param messageType  The type of the message.
     * @param pCallbackData  The debug message data.
     * @param pUserData  The user data.
     * @return VK_TRUE if the message is handled, VK_FALSE otherwise.
     */
    static VKAPI_ATTR VkBool32 VKAPI_CALL Callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                   VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                   const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                   void *pUserData);

    /**
     * @brief Populates the debug messenger create info.
     *
     * This function initializes the debug messenger create info structure
     * with the necessary parameters for setting up the debug messenger.
     *
     * @param createInfo  The debug messenger create info.
     */
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    /**
     * @brief Destroys the debug messenger.
     *
     * This function destroys the debug messenger.
     *
     * @param instance  The Vulkan instance.
     * @param pAllocator  The allocator.
     */
    void DestroyDebugUtilsMessengerEXT(const VkInstance &instance, const VkAllocationCallbacks *pAllocator);

  private:
    /**
     * @brief Creates the debug messenger.
     *
     * This function creates the debug messenger for the Vulkan instance.
     *
     * @param instance  The Vulkan instance.
     * @param pCreateInfo  The debug messenger create info.
     * @param pAllocator  The allocator.
     * @return The result of the debug messenger creation.
     */
    VkResult CreateDebugUtilsMessengerEXT(const VkInstance &instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator);

  private:
    VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;
    VkInstance _instance = VK_NULL_HANDLE;

    inline static std::unordered_map<VkDebugUtilsMessageSeverityFlagBitsEXT, ES::Utils::Log::Level> _severityMap = {
        {VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT, ES::Utils::Log::Level::info},
        {VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,    ES::Utils::Log::Level::info},
        {VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT, ES::Utils::Log::Level::warn},
        {VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,   ES::Utils::Log::Level::err }
    };
};

} // namespace ES::Plugin::Wrapper

#endif /* !DEBUGMESSENGER_HPP_ */
