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

#include <set>

namespace ES::Plugin::Wrapper {

/**
 * @brief LogicalDevice class.
 *
 * @example
 * @code
 * LogicalDevice device;
 * device.Create(physicalDevice);
 * device.Destroy();
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
     *
     * @param physicalDevice  The selected physical device.
     * @param surface  The Vulkan surface.
     */
    void Create(const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface);

    /**
     * @brief Destroys the logical device.
     *
     * This function destroys the logical device.
     */
    void Destroy() { vkDestroyDevice(_device, nullptr); }

    /**
     * @brief Gets the logical device.
     *
     * This function returns the logical device.
     *
     * @return The logical device.
     */
    [[nodiscard]] const VkDevice &Get() const { return _device; }

    /**
     * @brief Gets the present queue.
     *
     * This function returns the present queue.
     *
     * @return The present queue.
     */
    [[nodiscard]] const VkQueue &GetPresentQueue() { return _presentQueue; }

    /**
     * @brief Gets the graphics queue.
     *
     * This function returns the graphics queue.
     *
     * @return The graphics queue.
     */
    [[nodiscard]] const VkQueue &GetGraphicsQueue() { return _graphicsQueue; }

  private:
    VkDevice _device = VK_NULL_HANDLE;
    QueueFamilies _queueFamilies;
    VkQueue _graphicsQueue;
    VkQueue _presentQueue;
};

} // namespace ES::Plugin::Wrapper

#endif /* !LOGICALDEVICE_HPP_ */
