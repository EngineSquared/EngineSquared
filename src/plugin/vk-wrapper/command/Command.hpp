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
 * @file Command.hpp
 * @brief Command class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-24
 **************************************************************************/

#ifndef COMMAND_HPP_
#define COMMAND_HPP_

#include "QueueFamilies.hpp"

namespace ES::Plugin::Wrapper {

/**
 * @brief Command class.
 *
 *
 * @example
 * @code
 * @endcode
 */
class Command {
  public:
    struct CreateInfo {
        VkPhysicalDevice physicalDevice;
        VkSurfaceKHR surface;
        VkExtent2D swapChainExtent;
        VkRenderPass renderPass;
        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkPipeline graphicsPipeline;
    };

  public:
    void create(const VkDevice device, const CreateInfo info);

    void destroy(const VkDevice device);

  private:
    void record(VkCommandBuffer commandBuffer, uint32_t imageIndex, const VkRenderPass renderPass,
                const std::vector<VkFramebuffer> swapChainFramebuffers, const VkExtent2D swapChainExten,
                const VkPipeline graphicsPipeline);

  private:
    VkCommandPool _commandPool;
    QueueFamilies _queueFamilies;
    std::vector<VkCommandBuffer> _commandBuffers;
};

} // namespace ES::Plugin::Wrapper

#endif /* !COMMAND_HPP_ */
