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
 * @file GraphicsPipeline.hpp
 * @brief GraphicsPipeline class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-23
 **************************************************************************/

#ifndef GRAPHICSPIPELINE_HPP_
#define GRAPHICSPIPELINE_HPP_

#include "ShaderModule.hpp"

namespace ES::Plugin::Wrapper {

/**
 * @brief GraphicsPipeline class.
 *
 * @example
 * @code
 * GraphicsPipeline graphicsPipeline;
 * graphicsPipeline.create(device, swapChainExtent, renderPass, shaders);
 * graphicsPipeline.destroy(device);
 * @endcode
 */
class GraphicsPipeline {
  public:
    /**
     * @brief Creates a graphics pipeline.
     *
     * This function creates a graphics pipeline from the device, the swap chain extent, and the render pass.
     *
     * @param device  The Vulkan device.
     * @param swapChainExtent  The swap chain extent.
     * @param renderPass  The render pass.
     * @param shaders  The shader paths.
     */
    void create(const VkDevice &device, const VkExtent2D swapChainExtent, const VkRenderPass &renderPass,
                const ShaderModule::ShaderPaths &shaders);

    /**
     * @brief Destroys the graphics pipeline.
     *
     * This function destroys the graphics pipeline.
     *
     * @param device  The Vulkan device.
     */
    void destroy(const VkDevice &device);

    /**
     * @brief Gets the graphics pipeline.
     *
     * This function returns the graphics pipeline.
     *
     * @return VkPipeline  The graphics pipeline.
     */
    [[nodiscard]] const VkPipeline &get() const { return _graphicsPipeline; }

  private:
    VkPipelineLayout _pipelineLayout;
    VkPipeline _graphicsPipeline;
};

} // namespace ES::Plugin::Wrapper

#endif /* !GRAPHICSPIPELINE_HPP_ */
