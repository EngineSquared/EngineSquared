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
#include <vector>

#include "config.h"
#include "export.h"

namespace ES::Plugin::Wrapper {

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

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
 * debugMessenger.setupDebugMessenger(instance);
 * @endcode
 */
class DebugMessenger {
  public:
    DebugMessenger() = default;
    ~DebugMessenger();

    void setupDebugMessenger(const VkInstance instance);

    static VKAPI_ATTR VkBool32 VKAPI_CALL callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                   VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                   const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                   void *pUserData);

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    void DestroyDebugUtilsMessengerEXT(const VkInstance instance, const VkAllocationCallbacks *pAllocator);

  private:
    VkResult CreateDebugUtilsMessengerEXT(const VkInstance instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                          const VkAllocationCallbacks *pAllocator);

  private:
    VkDebugUtilsMessengerEXT _debugMessenger = VK_NULL_HANDLE;
    VkInstance _instance = VK_NULL_HANDLE;
};

} // namespace ES::Plugin::Wrapper

#endif /* !DEBUGMESSENGER_HPP_ */
