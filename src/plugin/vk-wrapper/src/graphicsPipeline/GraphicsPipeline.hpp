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

#include "Buffer.hpp"
#include "ShaderModule.hpp"

namespace ES::Plugin::Wrapper {

/**
 * @brief GraphicsPipeline class.
 *
 * @example
 * @code
 * GraphicsPipeline graphicsPipeline;
 * graphicsPipeline.Create(device, swapChainExtent, renderPass, shaders);
 * graphicsPipeline.Destroy(device);
 * @endcode
 */
class GraphicsPipeline {
  public:
    /**
     * @brief Creates a graphics pipeline.
     *
     * This function creates a graphics pipeline from the device and the render pass.
     *
     * @param device  The Vulkan device.
     * @param renderPass  The render pass.
     * @param shaders  The shader paths.
     * @param descriptorLayout  The descriptor layout.
     */
    void Create(const VkDevice &device, const VkRenderPass &renderPass, const ShaderModule::ShaderPaths &shaders,
                const VkDescriptorSetLayout &descriptorLayout);

    /**
     * @brief Destroys the graphics pipeline.
     *
     * This function destroys the graphics pipeline.
     *
     * @param device  The Vulkan device.
     */
    void Destroy(const VkDevice &device);

    /**
     * @brief Gets the graphics pipeline.
     *
     * This function returns the graphics pipeline.
     *
     * @return VkPipeline  The graphics pipeline.
     */
    [[nodiscard]] const VkPipeline &Get() const { return _graphicsPipeline; }

    /**
     * @brief Gets the pipeline layout.
     *
     * This function returns the pipeline layout.
     *
     * @return VkPipelineLayout  The pipeline layout.
     */
    [[nodiscard]] const VkPipelineLayout &GetLayout() const { return _pipelineLayout; }

  private:
    /**
     * @brief Sets up the color blend attachment.
     *
     * The color blend attachment in vulkan is used to blend the color of the fragment with the color of the
     * framebuffer. It is used to create effects like transparency.
     *
     * @param colorBlendAttachment  The color blend attachment.
     * @param enableBlend  The enable blend.
     */
    void SetupColorBlendAttachment(VkPipelineColorBlendAttachmentState &colorBlendAttachment,
                                   const VkBool32 enableBlend);

  private:
    VkPipelineLayout _pipelineLayout;
    VkPipeline _graphicsPipeline;
};

} // namespace ES::Plugin::Wrapper

#endif /* !GRAPHICSPIPELINE_HPP_ */
