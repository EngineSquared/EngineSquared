#include "Instance.hpp"

namespace ES::Plugin::Wrapper {

void Instance::Create(const std::string &applicationName)
{
    if (enableValidationLayers && !CheckValidationLayerSupport())
        throw VkWrapperError("validation layers requested, but not available!");

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = applicationName.c_str();
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 0);
    appInfo.pEngineName = "EngineSquared";
    appInfo.engineVersion = VK_MAKE_API_VERSION(VKWRAPPER_VERSION_MAJOR, VKWRAPPER_VERSION_MINOR,
                                                VKWRAPPER_VERSION_PATCH, VKWRAPPER_VERSION_TWEAK);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
#if VKWRAPPER_SYSTEM_MACOS
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
        createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

        _debugMessenger.PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }

    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
        throw VkWrapperError("failed to create instance!");
}

void Instance::Destroy(entt::resource_cache<Texture, TextureLoader> &textures)
{
    const auto &device = _logicalDevice.Get();

    CleanupSwapChain(device);

    _graphicsPipeline.Destroy(device);
    _renderPass.Destroy(device);
    _buffers.Destroy(device, _swapChain.GetSwapChainImages());
    _descriptorLayout.Destroy(device);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        vkDestroySemaphore(device, _renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(device, _imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(device, _inFlightFences[i], nullptr);
    }

    for (auto [id, res] : textures)
    {
        if (res)
            const_cast<Texture &>(*res).Destroy(device);
    }

    _command.Destroy(device);
    _logicalDevice.Destroy();

    if (enableValidationLayers)
        _debugMessenger.DestroyDebugUtilsMessengerEXT(_instance, nullptr);

    _surface.Destroy(_instance);
    vkDestroyInstance(_instance, nullptr);
}

bool Instance::CheckValidationLayerSupport() const
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    return std::ranges::all_of(VALIDATION_LAYERS, [&availableLayers](const char *layerName) {
        return std::ranges::any_of(availableLayers, [layerName](const VkLayerProperties &layerProperties) {
            return strcmp(layerName, layerProperties.layerName) == 0;
        });
    });
}

std::vector<const char *> Instance::GetRequiredExtensions() const
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#if VKWRAPPER_SYSTEM_MACOS
    extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

    if (enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

void Instance::SetupDebugMessenger()
{
    if (!enableValidationLayers)
        return;

    _debugMessenger.SetupDebugMessenger(_instance);
}

void Instance::CreateSurface(GLFWwindow *window) { _surface.Create(window, _instance); }

void Instance::SetupDevices()
{
    const auto &surface = _surface.Get();

    _physicalDevice.PickPhysicalDevice(_instance, surface);
    _logicalDevice.Create(_physicalDevice.Get(), surface);
}

void Instance::CreateSwapChainImages(const uint32_t width, const uint32_t height)
{
    const auto &device = _logicalDevice.Get();
    _currentFrame = 0;

    _swapChain.Create(device, _physicalDevice.Get(), _surface.Get(), width, height);
    _imageView.Create(device, _swapChain.GetSwapChainImages(), _swapChain.GetSurfaceFormat());
}

void Instance::CreateGraphicsPipeline(
    const ShaderModule::ShaderPaths &shaders, const entt::resource_cache<Texture, TextureLoader> &textures,
    const entt::resource_cache<Object::Component::Mesh, Object::Component::MeshLoader> &models)
{
    const auto &device = _logicalDevice.Get();
    const auto &extent = _swapChain.GetExtent();

    _renderPass.Create(device, _swapChain.GetSurfaceFormat().format);

    _descriptorLayout.Create(device);
    _graphicsPipeline.Create(device, _renderPass.Get(), shaders, _descriptorLayout.Get());

    const auto &renderPass = _renderPass.Get();

    Framebuffer::CreateInfo framebufferInfo{};
    framebufferInfo.swapChainExtent = extent;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.swapChainImageViews = _imageView.GetImageViews();

    _framebuffer.Create(device, framebufferInfo);

    const auto &physicalDevice = _physicalDevice.Get();

    Command::CreateInfo commandInfo{};
    commandInfo.physicalDevice = physicalDevice;
    commandInfo.surface = _surface.Get();
    commandInfo.swapChainExtent = extent;
    commandInfo.renderPass = renderPass;
    commandInfo.swapChainFramebuffers = _framebuffer.GetSwapChainFramebuffers();
    commandInfo.graphicsPipeline = _graphicsPipeline.Get();

    _command.Create(device, commandInfo);

    Buffers::CreateInfo buffersInfo{};
    buffersInfo.device = device;
    buffersInfo.physicalDevice = physicalDevice;
    buffersInfo.commandPool = _command.GetCommandPool();
    buffersInfo.graphicsQueue = _logicalDevice.GetGraphicsQueue();
    buffersInfo.swapChainImages = _swapChain.GetSwapChainImages();

    _buffers.Create(buffersInfo, textures);

    _descriptorLayout.CreateDescriptorPool(device);
    _descriptorLayout.CreateDescriptorSet(device, _buffers.GetUniformBuffers(),
                                          const_cast<Texture &>(*textures.begin()->second));

    _command.CreateCommandBuffers(device, _framebuffer.GetSwapChainFramebuffers());
}

void Instance::CreateSyncObjects()
{
    _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    const auto &device = _logicalDevice.Get();

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
    {
        if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(device, &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
            throw VkWrapperError("failed to create semaphores!");
    }
}

void Instance::RecreateSwapChain(const uint32_t width, const uint32_t height)
{
    const auto &device = _logicalDevice.Get();

    vkDeviceWaitIdle(device);

    CleanupSwapChain(device);

    CreateSwapChainImages(width, height);

    Framebuffer::CreateInfo framebufferInfo{};
    framebufferInfo.swapChainExtent = _swapChain.GetExtent();
    framebufferInfo.renderPass = _renderPass.Get();
    framebufferInfo.swapChainImageViews = _imageView.GetImageViews();

    _framebuffer.Create(device, framebufferInfo);
}

void Instance::CleanupSwapChain(const VkDevice &device)
{
    _framebuffer.Destroy(device);
    _imageView.Destroy(device);
    _swapChain.Destroy(device);
}

Result Instance::DrawNextImage()
{
    const auto &device = _logicalDevice.Get();

    vkWaitForFences(device, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(device, _swapChain.Get(), UINT64_MAX,
                                            _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
        return Result::NeedResize;
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        throw VkWrapperError("failed to acquire swap chain image!");

    _buffers.UpdateUniformBuffer(device, _swapChain.GetExtent(), _currentFrame);

    vkResetFences(device, 1, &_inFlightFences[_currentFrame]);

    Command::RecordInfo recordInfo{};
    recordInfo.currentFrame = _currentFrame;
    recordInfo.imageIndex = imageIndex;
    recordInfo.renderPass = _renderPass.Get();
    recordInfo.swapChainExtent = _swapChain.GetExtent();
    recordInfo.swapChainFramebuffers = _framebuffer.GetSwapChainFramebuffers();
    recordInfo.graphicsPipeline = _graphicsPipeline.Get();
    recordInfo.pipelineLayout = _graphicsPipeline.GetLayout();
    recordInfo.descriptorSet = _descriptorLayout.GetDescriptorSets()[_currentFrame];
    recordInfo.vertexBuffer = _buffers.GetVertexBuffer();
    recordInfo.indexBuffer = _buffers.GetIndexBuffer();

    _command.RecordBuffer(recordInfo);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    std::array<VkSemaphore, 1> waitSemaphores = {_imageAvailableSemaphores[_currentFrame]};
    std::array<VkPipelineStageFlags, 1> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages.data();

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_command.GetCommandBuffer(_currentFrame);

    std::array<VkSemaphore, 1> signalSemaphores = {_renderFinishedSemaphores[_currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    if (vkQueueSubmit(_logicalDevice.GetGraphicsQueue(), 1, &submitInfo, _inFlightFences[_currentFrame]) != VK_SUCCESS)
        throw VkWrapperError("failed to submit draw command buffer!");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores.data();

    std::array<VkSwapchainKHR, 1> swapChains = {_swapChain.Get()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains.data();
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(_logicalDevice.GetPresentQueue(), &presentInfo);

    vkQueueWaitIdle(_logicalDevice.GetPresentQueue());

#if (MAX_FRAMES_IN_FLIGHT & (MAX_FRAMES_IN_FLIGHT - 1)) == 0
    _currentFrame = (_currentFrame + 1) & (MAX_FRAMES_IN_FLIGHT - 1);
#else
    _currentFrame = _currentFrame + 1;
    _currentFrame *= _currentFrame < MAX_FRAMES_IN_FLIGHT;
#endif

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || _framebufferResized)
        return Result::NeedResize;
    else if (result != VK_SUCCESS)
        throw VkWrapperError("failed to present swap chain image!");

    return Result::Success;
}

} // namespace ES::Plugin::Wrapper
