#include "QueueFamilies.hpp"

namespace ES::Plugin::Wrapper {

void QueueFamilies::FindQueueFamilies(const VkPhysicalDevice &device, const VkSurfaceKHR &surface)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    uint32_t i = 0;

    for (const auto &queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            _indices.graphicsFamily = i;

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport)
            _indices.presentFamily = i;

        if (_indices.IsComplete())
            break;

        i++;
    }
}

} // namespace ES::Plugin::Wrapper
