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

#include "QueueFamilies.hpp"

#include <map>
#include <stdexcept>

namespace ES::Plugin::Wrapper {

/**
 * @brief PhysicalDevice class.
 *
 *
 * @example
 * @code
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
     */
    void pickPhysicalDevice(const VkInstance instance, const VkSurfaceKHR surface);

    [[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }

  private:
    [[nodiscard]] bool isDeviceSuitable(const VkPhysicalDevice device, const VkSurfaceKHR surface);
    [[nodiscard]] uint32_t rateDeviceSuitability(const VkPhysicalDevice device);

  private:
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    QueueFamilies _queueFamilies;
};

} // namespace ES::Plugin::Wrapper

#endif /* !PHYSICALDEVICE_HPP_ */
