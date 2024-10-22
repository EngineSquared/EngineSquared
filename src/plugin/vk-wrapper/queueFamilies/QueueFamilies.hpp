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
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;

        [[nodiscard]] bool isComplete() const { return graphicsFamily.has_value(); }
    };

  public:
    void findQueueFamilies(VkPhysicalDevice device);

    [[nodiscard]] bool isComplete() const { return _indices.isComplete(); }

  private:
    QueueFamilyIndices _indices;
};

} // namespace ES::Plugin::Wrapper

#endif /* !QUEUEFAMILIES_HPP_ */
