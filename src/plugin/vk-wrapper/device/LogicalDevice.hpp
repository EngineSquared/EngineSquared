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
 * @file LogicalDevice.hpp
 * @brief LogicalDevice class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-22
 **************************************************************************/

#ifndef LOGICALDEVICE_HPP_
#define LOGICALDEVICE_HPP_

#include "QueueFamilies.hpp"

namespace ES::Plugin::Wrapper {

/**
 * @brief LogicalDevice class.
 *
 *
 * @example
 * @code
 * Device device;
 * device.createLogicalDevice(physicalDevice);
 * @endcode
 */
class LogicalDevice {
  public:
    /**
     * @brief Creates a logical device from the selected physical device.
     *
     * This function creates a logical device, which is an abstraction
     * representing the GPU. It enables communication with the physical
     * device and allows the application to execute Vulkan commands.
     * The logical device is configured with specific features and
     * extensions required by the application.
     */
    void createLogicalDevice(VkPhysicalDevice physicalDevice);

  private:
    VkDevice device = VK_NULL_HANDLE;
    QueueFamilies _queueFamilies;
};

} // namespace ES::Plugin::Wrapper

#endif /* !LOGICALDEVICE_HPP_ */
