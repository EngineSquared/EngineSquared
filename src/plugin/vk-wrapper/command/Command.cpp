/*
** EPITECH PROJECT, 2024
** Title: EngineSquared
** Author: MasterLaplace
** Created: 2024-10-24
** File description:
** Command
*/

#include "Command.hpp"

namespace ES::Plugin::Wrapper {

void Command::create(const VkDevice device, const CreateInfo info)
{
    _queueFamilies.findQueueFamilies(info.physicalDevice, info.surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = _queueFamilies.getIndices().graphicsFamily.value();

    if (vkCreateCommandPool(device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS)
        throw std::runtime_error("failed to create command pool!");

    _commandBuffers.resize(info.swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = _commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) _commandBuffers.size();

    if (vkAllocateCommandBuffers(device, &allocInfo, _commandBuffers.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate command buffers!");
}

void Command::destroy(const VkDevice device) { vkDestroyCommandPool(device, _commandPool, nullptr); }

void Command::recordBuffer(const RecordInfo info)
{
    auto commandBuffer = _commandBuffers[info.currentFrame];

    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = info.renderPass;
    renderPassInfo.framebuffer = info.swapChainFramebuffers[info.imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = info.swapChainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, info.graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) info.swapChainExtent.width;
    viewport.height = (float) info.swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = info.swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("failed to record command buffer!");
}

} // namespace ES::Plugin::Wrapper
