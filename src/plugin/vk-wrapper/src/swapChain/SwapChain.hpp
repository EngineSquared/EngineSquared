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

#ifndef UINT32_MAX
#    include <limits>
#    define UINT32_MAX std::numeric_limits<uint32_t>::max()
#endif

namespace ES::Plugin::Wrapper {

/**
 * @brief SwapChain class.
 *
 * This class is used to create a swap chain for the Vulkan API. The swap chain
 * is used to present images to the screen. It is a queue of images that are
 * displayed on the screen. The swap chain is used to synchronize the rendering
 * of images with the refresh rate of the screen.
 *
 * @example
 * @code
 * SwapChain swapChain;
 * swapChain.Create(device, physicalDevice, surface, width, height);
 * swapChain.Destroy(device);
 * @endcode
 */
class SwapChain {
  public:
    /**
     * @brief Details of the swap chain support.
     *
     */
    struct SupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

  public:
    /**
     * @brief Query the support of the swap chain.
     *
     * @param device  The device.
     * @param surface  The surface.
     * @return SupportDetails  The details of the swap chain support.
     */
    [[nodiscard]] static SupportDetails QuerySupport(const VkPhysicalDevice &device, const VkSurfaceKHR &surface);

    /**
     * @brief Create the Swap Chain object.
     *
     * @param device  The device.
     * @param physicalDevice  The physical device.
     * @param surface  The surface.
     * @param width  The width.
     * @param height  The height.
     */
    void Create(const VkDevice &device, const VkPhysicalDevice &physicalDevice, const VkSurfaceKHR &surface,
                const uint32_t width, const uint32_t height);

    /**
     * @brief Destroy the Swap Chain object.
     *
     * @param device  The device.
     */
    void Destroy(const VkDevice &device) { vkDestroySwapchainKHR(device, _swapChain, nullptr); }

    /**
     * @brief Get the Swap Chain object.
     *
     * @return const VkSwapchainKHR  The swap chain.
     */
    [[nodiscard]] const VkSwapchainKHR &Get() const { return _swapChain; }

    /**
     * @brief Get the Details object for the swap chain support.
     *
     * @return const SupportDetails  The details of the swap chain support.
     */
    [[nodiscard]] const SupportDetails &GetDetails() const { return _supportDetails; }

    /**
     * @brief Get the Surface Format object.
     *
     * @return VkSurfaceFormatKHR  The surface format.
     */
    [[nodiscard]] VkSurfaceFormatKHR GetSurfaceFormat() const { return _surfaceFormat; }

    /**
     * @brief Get the Present Mode object.
     *
     * @return VkPresentModeKHR  The present mode.
     */
    [[nodiscard]] VkPresentModeKHR GetPresentMode() const { return _presentMode; }

    /**
     * @brief Get the Extent object.
     *
     * @return VkExtent2D  The extent.
     */
    [[nodiscard]] VkExtent2D GetExtent() const { return _extent; }

    /**
     * @brief Get the Swap Chain Images object.
     *
     * @return const std::vector<VkImage>&  The swap chain images.
     */
    [[nodiscard]] const std::vector<VkImage> &GetSwapChainImages() const { return _swapChainImages; }

  private:
    /**
     * @brief Choose the swap surface format.
     *
     * @param availableFormats  The available formats.
     * @return VkSurfaceFormatKHR  The chosen format.
     */
    [[nodiscard]] VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

    /**
     * @brief Choose the swap present mode.
     *
     * @param availablePresentModes  The available present modes.
     * @return VkPresentModeKHR  The chosen present mode.
     */
    [[nodiscard]] VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    /**
     * @brief Choose the swap extent.
     *
     * @param capabilities  The capabilities.
     * @param width  The width.
     * @param height  The height.
     * @return VkExtent2D  The chosen extent.
     */
    [[nodiscard]] VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const uint32_t width,
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
