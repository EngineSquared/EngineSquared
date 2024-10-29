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

void Instance::create(const std::string &applicationName)
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
    createInfo.enabledExtensionCount = (uint32_t) extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();
#if VKWRAPPER_SYSTEM_MACOS
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = (uint32_t) validationLayers.size();
        createInfo.ppEnabledLayerNames = validationLayers.data();

        _debugMessenger.populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
    }

    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
        throw std::runtime_error("failed to create instance!");
}

void Instance::destroy()
{
    VkDevice device = _logicalDevice.get();

    cleanupSwapChain(device);
    _graphicsPipeline.destroy(device);
    _renderPass.destroy(device);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vkDestroySemaphore(device, _renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, _imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, _inFlightFences[i], nullptr);
    }

    _command.destroy(device);
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
    auto surface = _surface.get();

    _physicalDevice.pickPhysicalDevice(_instance, surface);
    _logicalDevice.create(_physicalDevice.get(), surface);
}

void Instance::createSwapChainImages(const uint32_t width, const uint32_t height)
{
    auto device = _logicalDevice.get();
    _currentFrame = 0;

    _swapChain.create(device, _physicalDevice.get(), _surface.get(), width, height);
    _imageView.create(device, _swapChain.getSwapChainImages(), _swapChain.getSurfaceFormat());
}

void Instance::createGraphicsPipeline()
{
    auto device = _logicalDevice.get();
    auto extent = _swapChain.getExtent();

    _renderPass.create(device, _swapChain.getSurfaceFormat().format);

    _graphicsPipeline.create(device, extent, _renderPass.get());

    auto renderPass = _renderPass.get();

    Framebuffer::CreateInfo framebufferInfo{};
    framebufferInfo.swapChainExtent = extent;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.swapChainImageViews = _imageView.getImageViews();

    _framebuffer.create(device, framebufferInfo);

    Command::CreateInfo commandInfo{};
    commandInfo.physicalDevice = _physicalDevice.get();
    commandInfo.surface = _surface.get();
    commandInfo.swapChainExtent = extent;
    commandInfo.renderPass = renderPass;
    commandInfo.swapChainFramebuffers = _framebuffer.getSwapChainFramebuffers();
    commandInfo.graphicsPipeline = _graphicsPipeline.get();

    _command.create(device, commandInfo);
}

void Instance::createSyncObjects()
{
    _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    auto device = _logicalDevice.get();

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
            throw std::runtime_error("failed to create semaphores!");
    }
}

void Instance::recreateSwapChain(uint32_t width, uint32_t height)
{
    auto device = _logicalDevice.get();

    vkDeviceWaitIdle(device);

    cleanupSwapChain(device);

    createSwapChainImages(width, height);

    Framebuffer::CreateInfo framebufferInfo{};
    framebufferInfo.swapChainExtent = _swapChain.getExtent();
    framebufferInfo.renderPass = _renderPass.get();
    framebufferInfo.swapChainImageViews = _imageView.getImageViews();

    _framebuffer.create(device, framebufferInfo);
}

void Instance::cleanupSwapChain(const VkDevice device)
{
    _framebuffer.destroy(device);
    _imageView.destroy(device);
    _swapChain.destroy(device);
}

Result Instance::drawNextImage()
{
    auto device = _logicalDevice.get();

    vkWaitForFences(device, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(device, _swapChain.get(), UINT64_MAX,
                                            _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
        return Result::Failure;
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw std::runtime_error("failed to acquire swap chain image!");

    vkResetFences(device, 1, &_inFlightFences[_currentFrame]);

    Command::RecordInfo recordInfo{};
    recordInfo.currentFrame = _currentFrame;
    recordInfo.imageIndex = imageIndex;
    recordInfo.renderPass = _renderPass.get();
    recordInfo.swapChainExtent = _swapChain.getExtent();
    recordInfo.swapChainFramebuffers = _framebuffer.getSwapChainFramebuffers();
    recordInfo.graphicsPipeline = _graphicsPipeline.get();

    _command.recordBuffer(recordInfo);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {_imageAvailableSemaphores[_currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_command.getCommandBuffer(_currentFrame);

    VkSemaphore signalSemaphores[] = {_renderFinishedSemaphores[_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(_logicalDevice.getGraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]) != VK_SUCCESS)
        throw std::runtime_error("failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {_swapChain.get()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(_logicalDevice.getPresentQueue(), &presentInfo);

    vkQueueWaitIdle(_logicalDevice.getPresentQueue());

#if (MAX_FRAMES_IN_FLIGHT & (MAX_FRAMES_IN_FLIGHT - 1)) == 0
    _currentFrame = (_currentFrame + 1) & (MAX_FRAMES_IN_FLIGHT - 1);
#else
    _currentFrame = _currentFrame + 1;
    _currentFrame *= _currentFrame < MAX_FRAMES_IN_FLIGHT;
#endif

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferResized)
        return Result::Failure;
    else if (result != VK_SUCCESS)
        throw std::runtime_error("failed to present swap chain image!");

    return Result::Success;
}

} // namespace ES::Plugin::Wrapper
