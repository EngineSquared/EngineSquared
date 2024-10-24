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
 * @file ImageView.hpp
 * @brief ImageView class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-23
 **************************************************************************/

#ifndef IMAGEVIEW_HPP_
#define IMAGEVIEW_HPP_

#include "SwapChain.hpp"

namespace ES::Plugin::Wrapper {

/**
 * @brief ImageView class.
 *
 *
 * @example
 * @code
 * @endcode
 */
class ImageView {
  public:
    /** @brief Creates image views for the provided swap chain images.
     *
     * This function initializes image views for each image in the swap chain.
     *
     * @param device The Vulkan logical device used to create the image views.
     * @param swapChainImages A vector of Vulkan images representing the swap chain images.
     * @param surfaceFormat The surface format used to create the image views.
     */
    void create(const VkDevice device, std::vector<VkImage> swapChainImages, VkSurfaceFormatKHR surfaceFormat);

    /** @brief Destroys the image views.
     *
     * This function destroys the image views created by the create function.
     * The image views are destroyed using the Vulkan logical device provided
     * when the image views were created.
     *
     * @param device The Vulkan logical device used to destroy the image views.
     */
    void destroy(const VkDevice device);

    /**
     * @brief Get the Image Views object.
     *
     * @return const std::vector<VkImageView> The image views.
     */
    [[nodiscard]] const std::vector<VkImageView> &getImageViews() const { return _swapChainImageViews; }

  private:
    std::vector<VkImageView> _swapChainImageViews;
};

} // namespace ES::Plugin::Wrapper

#endif /* !IMAGEVIEW_HPP_ */
