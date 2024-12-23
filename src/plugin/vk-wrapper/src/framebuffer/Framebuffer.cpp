#include "Framebuffer.hpp"

namespace ES::Plugin::Wrapper {

void Framebuffer::Create(const VkDevice &device, const CreateInfo &info)
{
    _swapChainFramebuffers.resize(info.swapChainImageViews.size());

    for (uint32_t i = 0; i < info.swapChainImageViews.size(); ++i)
    {
        std::array<VkImageView, 1> attachments = {info.swapChainImageViews[i]};

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = info.renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = info.swapChainExtent.width;
        framebufferInfo.height = info.swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS)
            throw VkWrapperError("failed to create framebuffer!");
    }
}

void Framebuffer::Destroy(const VkDevice &device) const
{
    for (auto framebuffer : _swapChainFramebuffers)
        vkDestroyFramebuffer(device, framebuffer, nullptr);
}

} // namespace ES::Plugin::Wrapper
