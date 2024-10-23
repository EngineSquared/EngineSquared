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
 * @file SwapChain.hpp
 * @brief SwapChain class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-23
 **************************************************************************/

#ifndef SWAPCHAIN_HPP_
#define SWAPCHAIN_HPP_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

namespace ES::Plugin::Wrapper {

/**
 * @brief SwapChain class.
 *
 *
 * @example
 * @code
 * @endcode
 */
class SwapChain {
  public:
    struct SupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

  public:
    static [[nodiscard]] SupportDetails querySupport(const VkPhysicalDevice device, const VkSurfaceKHR surface);
};

} // namespace ES::Plugin::Wrapper

#endif /* !SWAPCHAIN_HPP_ */
