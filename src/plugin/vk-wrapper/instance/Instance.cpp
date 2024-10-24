/*
** EPITECH PROJECT, 2024
** Title: EngineSquared
** Author: MasterLaplace
** Created: 2024-10-15
** File description:
** Instance
*/

#include "Instance.hpp"

namespace ES::Plugin::Wrapper {

Instance::Instance(const std::string &applicationName)
{
    if (enableValidationLayers && !CheckValidationLayerSupport())
        throw std::runtime_error("validation layers requested, but not available!");

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = applicationName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.pEngineName = "EngineSquared";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
#if VKWRAPPER_SYSTEM_MACOS
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        _debugMessenger.populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
    }

    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
        throw std::runtime_error("failed to create instance!");
}

Instance::~Instance()
{
    VkDevice device = _logicalDevice.get();

    _framebuffer.destroy(device);
    _graphicsPipeline.destroy(device);
    _renderPass.destroy(device);
    _imageView.destroy(device);
    _swapChain.destroy(device);
    _logicalDevice.destroy();

    if (enableValidationLayers)
        _debugMessenger.DestroyDebugUtilsMessengerEXT(_instance, nullptr);

    _surface.destroy(_instance);
    vkDestroyInstance(_instance, nullptr);
}

bool Instance::CheckValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }

    return true;
}

std::vector<const char *> Instance::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#if VKWRAPPER_SYSTEM_MACOS
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

void Instance::setupDebugMessenger()
{
    if (!enableValidationLayers)
        return;

    _debugMessenger.setupDebugMessenger(_instance);
}

void Instance::createSurface(GLFWwindow *window) { _surface.create(window, _instance); }

void Instance::setupDevices()
{
    _physicalDevice.pickPhysicalDevice(_instance, _surface.get());
    _logicalDevice.create(_physicalDevice.get(), _surface.get());
}

void Instance::createSwapChainImages(const uint32_t width, const uint32_t height)
{
    _swapChain.create(_physicalDevice.get(), _logicalDevice.get(), _surface.get(), width, height);
    _imageView.create(_logicalDevice.get(), _swapChain.getSwapChainImages(), _swapChain.getSurfaceFormat());
}

void Instance::createGraphicsPipeline()
{
    _renderPass.create(_logicalDevice.get(), _swapChain.getSurfaceFormat().format);
    _graphicsPipeline.create(_logicalDevice.get(), _swapChain.getExtent(), _renderPass.get());

    Framebuffer::CreateInfo framebufferInfo{};
    framebufferInfo.swapChainExtent = _swapChain.getExtent();
    framebufferInfo.renderPass = _renderPass.get();
    framebufferInfo.swapChainImageViews = _imageView.getImageViews();

    _framebuffer.create(_logicalDevice.get(), framebufferInfo);

    Command::CreateInfo commandInfo{};
    commandInfo.physicalDevice = _physicalDevice.get();
    commandInfo.surface = _surface.get();
    commandInfo.swapChainExtent = _swapChain.getExtent();
    commandInfo.renderPass = _renderPass.get();
    commandInfo.swapChainFramebuffers = _framebuffer.getSwapChainFramebuffers();
    commandInfo.graphicsPipeline = _graphicsPipeline.get();

    _command.create(_logicalDevice.get(), commandInfo);
}

void Instance::createSemaphores()
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(_logicalDevice.get(), &semaphoreInfo, nullptr, &_imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(_logicalDevice.get(), &semaphoreInfo, nullptr, &_renderFinishedSemaphore) != VK_SUCCESS)
        throw std::runtime_error("failed to create semaphores!");
}

} // namespace ES::Plugin::Wrapper
