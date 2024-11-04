#include "ImageView.hpp"

namespace ES::Plugin::Wrapper {

void ImageView::Create(const VkDevice &device, const std::vector<VkImage> &swapChainImages,
                       const VkSurfaceFormatKHR surfaceFormat, const uint32_t layerCount)
{
    _swapChainImageViews.resize(swapChainImages.size() * layerCount);

    for (uint32_t i = 0; i < swapChainImages.size(); ++i)
    {
        for (uint32_t layer = 0; layer < layerCount; ++layer)
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = surfaceFormat.format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = layer;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(device, &createInfo, nullptr, &_swapChainImageViews[i * layerCount + layer]) !=
                VK_SUCCESS)
                throw VkWrapperError("failed to create image views!");
        }
    }
}

void ImageView::Destroy(const VkDevice &device)
{
    for (const auto &imageView : _swapChainImageViews)
        vkDestroyImageView(device, imageView, nullptr);
}

} // namespace ES::Plugin::Wrapper
