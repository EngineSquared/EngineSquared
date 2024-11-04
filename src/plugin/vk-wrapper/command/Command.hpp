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
#include "Buffer.hpp"

namespace ES::Plugin::Wrapper {

const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

/**
 * @brief Command class.
 *
 * @example
 * @code
 * Command command;
 * Command::CreateInfo info = {
 *    .physicalDevice = physicalDevice.Get(),
 *    .surface = surface.Get(),
 *    .swapChainExtent = swapChain.GetExtent(),
 *    .renderPass = renderPass.Get(),
 *    .swapChainFramebuffers = framebuffer.GetSwapChainFramebuffers(),
 *    .graphicsPipeline = graphicsPipeline.Get(),
 * };
 * command.Create(device, info);
 * command.Destroy(device);
 * @endcode
 */
class Command {
  public:
    /**
     * @brief CreateInfo struct.
     *
     * This struct holds the information required to create a command pool.
     *
     * @var VkPhysicalDevice CreateInfo::physicalDevice  The physical device.
     * @var VkSurfaceKHR CreateInfo::surface  The surface.
     * @var VkExtent2D CreateInfo::swapChainExtent  The swap chain extent.
     * @var VkRenderPass CreateInfo::renderPass  The render pass.
     * @var std::vector<VkFramebuffer> CreateInfo::swapChainFramebuffers  The swap chain framebuffers.
     * @var VkPipeline CreateInfo::graphicsPipeline  The graphics pipeline.
     */
    struct CreateInfo {
        VkPhysicalDevice physicalDevice;
        VkSurfaceKHR surface;
        VkExtent2D swapChainExtent;
        VkRenderPass renderPass;
        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkPipeline graphicsPipeline;
    };

    /**
     * @brief RecordInfo struct.
     *
     * This struct holds the information required to record a command buffer.
     *
     * @var uint32_t RecordInfo::currentFrame  The current frame.
     * @var uint32_t RecordInfo::imageIndex  The image index.
     * @var VkRenderPass RecordInfo::renderPass  The render pass.
     * @var VkExtent2D RecordInfo::swapChainExtent  The swap chain extent.
     * @var std::vector<VkFramebuffer> RecordInfo::swapChainFramebuffers  The swap chain framebuffers.
     * @var VkPipeline RecordInfo::graphicsPipeline  The graphics pipeline.
     * @var VkBuffer RecordInfo::vertexBuffer  The vertex buffer.
     * @var VkBuffer RecordInfo::indexBuffer  The index buffer.
     */
    struct RecordInfo {
        uint32_t currentFrame;
        uint32_t imageIndex;
        VkRenderPass renderPass;
        VkExtent2D swapChainExtent;
        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkPipeline graphicsPipeline;
        VkBuffer vertexBuffer;
        VkBuffer indexBuffer;
    };

  public:
    /**
     * @brief Creates a command pool and command buffers.
     *
     * This function creates a command pool from the device and the queue families.
     * It also creates command buffers using the command pool.
     *
     * @param device  The Vulkan device.
     * @param queueFamilies  The queue families.
     */
    void Create(const VkDevice &device, const CreateInfo &info);

    /**
     * @brief Creates command buffers.
     *
     * This function creates command buffers using the command pool.
     *
     * @param device  The Vulkan device.
     */
    void CreateCommandBuffers(const VkDevice &device);

    /**
     * @brief Destroys the command pool.
     *
     * This function destroys the command pool.
     *
     * @param device  The Vulkan device.
     */
    void Destroy(const VkDevice &device);

    /**
     * @brief Records a command buffer.
     *
     * This function records a command buffer from the record info.
     *
     * @param info  The record info.
     */
    void RecordBuffer(const RecordInfo &info);

    /**
     * @brief Gets the command pool.
     *
     * This function returns the command pool.
     *
     * @return The command pool.
     */
    [[nodiscard]] const VkCommandPool &GetCommandPool() { return _commandPool; }

    /**
     * @brief Gets the command buffer.
     *
     * This function returns the command buffer.
     *
     * @param imageIndex  The image index.
     * @return The command buffer.
     */
    [[nodiscard]] const VkCommandBuffer &GetCommandBuffer(const uint32_t imageIndex)
    {
        return _commandBuffers[imageIndex];
    }

  private:
    VkCommandPool _commandPool;
    QueueFamilies _queueFamilies;
    std::vector<VkCommandBuffer> _commandBuffers;
};

} // namespace ES::Plugin::Wrapper

#endif /* !COMMAND_HPP_ */
