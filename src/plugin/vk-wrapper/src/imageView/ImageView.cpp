#include "ImageView.hpp"

namespace ES::Plugin::Wrapper {

void ImageView::Create(const VkDevice &device, const std::vector<VkImage> &swapChainImages,
                       const VkSurfaceFormatKHR surfaceFormat, const uint32_t layerCount)
{
    _swapChainImageViews.resize(swapChainImages.size() * layerCount);

    for (size_t i = 0; i < swapChainImages.size(); ++i)
    {
        for (uint32_t layer = 0; layer < layerCount; ++layer)
        {
            _swapChainImageViews[i * layerCount + layer] =
                CreateImageView(device, swapChainImages[i], surfaceFormat.format, layer);
        }
    }
}

void ImageView::Destroy(const VkDevice &device) const
{
    for (const auto &imageView : _swapChainImageViews)
        vkDestroyImageView(device, imageView, nullptr);
}

VkImageView ImageView::CreateImageView(const VkDevice &device, VkImage image, VkFormat format, uint32_t layer)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = layer;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView{};
    if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        throw VkWrapperError("failed to create image view!");

    return imageView;
}

} // namespace ES::Plugin::Wrapper
