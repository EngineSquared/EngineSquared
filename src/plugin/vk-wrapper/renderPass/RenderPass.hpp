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
 * @file RenderPass.hpp
 * @brief RenderPass class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-24
 **************************************************************************/

#ifndef RENDERPASS_HPP_
#define RENDERPASS_HPP_

#include "DebugMessenger.hpp"

namespace ES::Plugin::Wrapper {

/**
 * @brief RenderPass class.
 *
 * @example
 * @code
 * RenderPass renderPass;
 * renderPass.Create(device, swapChainImageFormat);
 * renderPass.Destroy(device);
 * @endcode
 */
class RenderPass {
  public:
    /**
     * @brief Creates a render pass.
     *
     * This function creates a render pass from the device and the swap chain image format.
     *
     * @param device  The Vulkan device.
     * @param swapChainImageFormat  The swap chain image format.
     */
    void Create(const VkDevice &device, const VkFormat swapChainImageFormat);

    /**
     * @brief Destroys the render pass.
     *
     * This function destroys the render pass.
     *
     * @param device  The Vulkan device.
     */
    void Destroy(const VkDevice &device);

    /**
     * @brief Gets the render pass.
     *
     * This function returns the render pass.
     *
     * @return The render pass.
     */
    [[nodiscard]] const VkRenderPass &Get() const { return _renderPass; }

  private:
    VkRenderPass _renderPass;
};

} // namespace ES::Plugin::Wrapper

#endif /* !RENDERPASS_HPP_ */
