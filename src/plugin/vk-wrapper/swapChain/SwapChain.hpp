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

#include "QueueFamilies.hpp"

#include <algorithm>
#include <cstdint>

#ifndef UINT32_MAX
#include <limits>
#define UINT32_MAX std::numeric_limits<uint32_t>::max()
#endif

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

    void create(const VkPhysicalDevice physicalDevice, const VkDevice device, const VkSurfaceKHR surface,
                const uint32_t width, const uint32_t height);

    void destroy(const VkDevice device) { vkDestroySwapchainKHR(device, _swapChain, nullptr); }

    /**
     * @brief Get the Details object for the swap chain support.
     *
     * @return const SupportDetails  The details of the swap chain support.
     */
    [[nodiscard]] const SupportDetails getDetails() const { return _supportDetails; }

    /**
     * @brief Get the Surface Format object.
     *
     * @return const VkSurfaceFormatKHR  The surface format.
     */
    [[nodiscard]] const VkSurfaceFormatKHR getSurfaceFormat() const { return _surfaceFormat; }

    /**
     * @brief Get the Present Mode object.
     *
     * @return const VkPresentModeKHR  The present mode.
     */
    [[nodiscard]] const VkPresentModeKHR getPresentMode() const { return _presentMode; }

    /**
     * @brief Get the Extent object.
     *
     * @return const VkExtent2D  The extent.
     */
    [[nodiscard]] const VkExtent2D getExtent() const { return _extent; }

    /**
     * @brief Get the Swap Chain Images object.
     *
     * @return const std::vector<VkImage>  The swap chain images.
     */
    [[nodiscard]] const std::vector<VkImage> getSwapChainImages() const { return _swapChainImages; }

  private:
    [[nodiscard]] VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    [[nodiscard]] VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    [[nodiscard]] VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const uint32_t width,
                                              const uint32_t height);

  private:
    SupportDetails _supportDetails;
    VkSwapchainKHR _swapChain;
    std::vector<VkImage> _swapChainImages;
    VkSurfaceFormatKHR _surfaceFormat;
    VkPresentModeKHR _presentMode;
    VkExtent2D _extent;
    QueueFamilies _queueFamilies;
};

} // namespace ES::Plugin::Wrapper

#endif /* !SWAPCHAIN_HPP_ */
