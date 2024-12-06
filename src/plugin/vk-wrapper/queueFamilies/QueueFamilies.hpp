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
 * @file QueueFamilies.hpp
 * @brief QueueFamilies class declaration.
 *
 * This class is used to find the queue families of a physical device.
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-21
 **************************************************************************/

#ifndef QUEUEFAMILIES_HPP_
#define QUEUEFAMILIES_HPP_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>
#include <vector>

namespace ES::Plugin::Wrapper {

const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

/**
 * @brief QueueFamilies class.
 *
 *
 * @example
 * @code
 * @endcode
 */
class QueueFamilies {
  public:
    /**
     * @struct QueueFamilyIndices
     * @brief Holds indices for Vulkan queue families.
     *
     * This structure contains optional indices for graphics and present queue families.
     * It provides a method to check if the graphics family index and the present family has been set.
     *
     * @var std::optional<uint32_t> QueueFamilyIndices::graphicsFamily
     * Optional index for the graphics queue family.
     * Check if the physical device supports graphics operations.
     *
     * @var std::optional<uint32_t> QueueFamilyIndices::presentFamily
     * Optional index for the present queue family.
     * Check if WSI is supported by the physical device.
     *
     * @fn bool QueueFamilyIndices::isComplete() const
     * @brief Checks if the graphics family index and the present family has been set.
     * @return True if the graphics family index and the present family has a value, false otherwise.
     */
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        [[nodiscard]] bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
    };

  public:
    /**
     * @brief Finds the queue families for a given Vulkan physical device.
     *
     * This function identifies the queue families that are supported by the specified
     * Vulkan physical device. Queue families are groups of queues that support a
     * particular set of operations, such as graphics, compute, or transfer operations.
     *
     * @param device The Vulkan physical device for which to find the queue families.
     */
    void findQueueFamilies(const VkPhysicalDevice device, const VkSurfaceKHR surface);

    [[nodiscard]] bool isComplete() const { return _indices.isComplete(); }

  private:
    QueueFamilyIndices _indices;
};

} // namespace ES::Plugin::Wrapper

#endif /* !QUEUEFAMILIES_HPP_ */
