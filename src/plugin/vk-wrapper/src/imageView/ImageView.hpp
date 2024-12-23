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
 * ImageView imageView;
 * imageView.Create(device, swapChainImages, surfaceFormat);
 * imageView.Destroy(device);
 * @endcode
 */
class ImageView {
  public:
    /** @brief Creates image views for the provided swap chain images.
     *
     * This function initializes image views for each image in the swap chain.
     * This function is used for stereoscopic 3D applications. The layer count
     * is used to create multiple image views for each image in the swap chain.
     * The number of image views created is equal to the number of swap chain images
     * multiplied by the layer count.
     * The layer count is typically 2 for stereoscopic 3D applications.
     * The layer count is 1 for non-stereoscopic applications.
     *
     * @param device  The Vulkan logical device used to create the image views.
     * @param swapChainImages  A vector of Vulkan images representing the swap chain images.
     * @param surfaceFormat  The surface format used to create the image views.
     * @param layerCount  The number of layers in the image views. This is used for
     *                    stereoscopic 3D applications. The default value is 1.
     */
    void Create(const VkDevice &device, const std::vector<VkImage> &swapChainImages,
                const VkSurfaceFormatKHR surfaceFormat, const uint32_t layerCount = 1);

    /** @brief Destroys the image views.
     *
     * This function destroys the image views created by the create function.
     * The image views are destroyed using the Vulkan logical device provided
     * when the image views were created.
     *
     * @param device  The Vulkan logical device used to destroy the image views.
     */
    void Destroy(const VkDevice &device) const;

    /**
     * @brief Get the Image Views object.
     *
     * @return const std::vector<VkImageView>  The image views.
     */
    [[nodiscard]] const std::vector<VkImageView> &GetImageViews() const { return _swapChainImageViews; }

  private:
    std::vector<VkImageView> _swapChainImageViews;
};

} // namespace ES::Plugin::Wrapper

#endif /* !IMAGEVIEW_HPP_ */
