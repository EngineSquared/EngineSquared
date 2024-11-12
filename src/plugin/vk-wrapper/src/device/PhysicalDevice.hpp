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
 * @file PhysicalDevice.hpp
 * @brief PhysicalDevice class declaration.
 *
 * This class is used to check if a physical device is suitable for the application.
 * It also checks if the device has the required queue families.
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-21
 **************************************************************************/

#ifndef PHYSICALDEVICE_HPP_
#define PHYSICALDEVICE_HPP_

#include "SwapChain.hpp"

#include <map>
#include <set>

namespace ES::Plugin::Wrapper {

/**
 * @brief PhysicalDevice class.
 *
 * This class is used to check if a physical device is suitable for the application.
 *
 * @example
 * @code
 * PhysicalDevice physicalDevice;
 * physicalDevice.PickPhysicalDevice(instance);
 * @endcode
 */
class PhysicalDevice {
  public:
    /**
     * @brief Picks a suitable physical device (GPU) for Vulkan operations.
     *
     * This function selects a physical device that meets the requirements
     * for running Vulkan applications. It evaluates available GPUs and
     * chooses the most appropriate one based on criteria such as support
     * for required features and extensions.
     *
     * @param instance  The Vulkan instance.
     * @param surface  The Vulkan surface.
     */
    void PickPhysicalDevice(const VkInstance &instance, const VkSurfaceKHR &surface);

    /**
     * @brief Gets the physical device.
     *
     * This function returns the physical device.
     *
     * @return The physical device.
     */
    [[nodiscard]] const VkPhysicalDevice &Get() const { return _physicalDevice; }

  private:
    /**
     * @brief Checks if the physical device is suitable for the application.
     *
     * This function verifies if the physical device meets the requirements
     * for running Vulkan applications. It checks if the device supports
     * the necessary features and extensions.
     *
     * @param device  The physical device to check.
     * @param surface  The Vulkan surface.
     * @return true if the device is suitable, false otherwise.
     */
    [[nodiscard]] bool IsDeviceSuitable(const VkPhysicalDevice &device, const VkSurfaceKHR &surface);

    /**
     * @brief Checks if the physical device supports the required extensions.
     *
     * This function verifies if the physical device supports the necessary
     * extensions for running Vulkan applications.
     *
     * @param device  The physical device to check.
     * @return true if the device supports the required extensions, false otherwise.
     */
    [[nodiscard]] bool CheckDeviceExtensionSupport(const VkPhysicalDevice &device);

    /**
     * @brief Rates the suitability of the physical device.
     *
     * This function evaluates the suitability of the physical device based on
     * various criteria such as the device type, supported features, and extensions.
     *
     * @param device  The physical device to rate.
     * @return The suitability rating of the device.
     */
    [[nodiscard]] uint32_t RateDeviceSuitability(const VkPhysicalDevice &device);

  private:
    VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;
    QueueFamilies _queueFamilies;
};

} // namespace ES::Plugin::Wrapper

#endif /* !PHYSICALDEVICE_HPP_ */
