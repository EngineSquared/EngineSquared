#include "DebugMessenger.hpp"

namespace ES::Plugin::Wrapper {

DebugMessenger::~DebugMessenger()
{
    if (enableValidationLayers)
        DestroyDebugUtilsMessengerEXT(_instance, nullptr);
}

void DebugMessenger::SetupDebugMessenger(const VkInstance &instance)
{
    if (!enableValidationLayers)
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr) != VK_SUCCESS)
        throw VkWrapperError("failed to set up debug messenger!");
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessenger::Callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData)
{
    if (_severityMap.find(messageSeverity) != _severityMap.end())
        ES::Utils::Log::Log(_severityMap[messageSeverity], "validation layer: " + std::string(pCallbackData->pMessage));
    else
        ES::Utils::Log::Log(ES::Utils::Log::Level::info, "validation layer: " + std::string(pCallbackData->pMessage));
    return VK_FALSE;
}

void DebugMessenger::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo)
{
    if (!enableValidationLayers)
        return;

    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugMessenger::Callback;
}

VkResult DebugMessenger::CreateDebugUtilsMessengerEXT(const VkInstance &instance,
                                                      const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                                      const VkAllocationCallbacks *pAllocator)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
        return func(instance, pCreateInfo, pAllocator, &_debugMessenger);

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DebugMessenger::DestroyDebugUtilsMessengerEXT(const VkInstance &instance, const VkAllocationCallbacks *pAllocator)
{
    if (!enableValidationLayers)
        return;

    auto func =
        (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
        func(instance, _debugMessenger, pAllocator);
}

} // namespace ES::Plugin::Wrapper
