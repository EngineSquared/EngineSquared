#include "system/initialization/CreateDevice.hpp"
#include "exception/DeviceCreationError.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"

namespace Plugin::Graphic::System {
static void SetupDeviceDescriptor(wgpu::DeviceDescriptor &deviceDesc,
                                  Plugin::Graphic::Resource::GraphicSettings &settings)
{
    deviceDesc.label = wgpu::StringView("Core Device");
    deviceDesc.requiredFeatureCount = settings.GetRequiredFeatures().size();
    deviceDesc.requiredFeatures = settings.GetRequiredFeatures().data();
    deviceDesc.requiredLimits = dynamic_cast<wgpu::Limits *>(&settings.GetWantedLimits());
    deviceDesc.defaultQueue.nextInChain = nullptr;
    deviceDesc.defaultQueue.label = wgpu::StringView("The default queue");
    deviceDesc.deviceLostCallbackInfo = {};
    deviceDesc.deviceLostCallbackInfo.nextInChain = nullptr;
    deviceDesc.deviceLostCallbackInfo.mode = wgpu::CallbackMode::AllowProcessEvents;
    deviceDesc.deviceLostCallbackInfo.callback = [](WGPUDevice const *device, WGPUDeviceLostReason reason,
                                                    WGPUStringView message, WGPU_NULLABLE void *userdata1,
                                                    WGPU_NULLABLE void *userdata2) {
        Log::Error(fmt::format("Device lost: reason {:x} ({})", static_cast<uint32_t>(reason),
                               std::string(message.data, message.length)));
    };
    deviceDesc.uncapturedErrorCallbackInfo = {};
    deviceDesc.uncapturedErrorCallbackInfo.nextInChain = nullptr;
    deviceDesc.uncapturedErrorCallbackInfo.callback = [](WGPUDevice const *device, WGPUErrorType type,
                                                         WGPUStringView message, WGPU_NULLABLE void *userdata1,
                                                         WGPU_NULLABLE void *userdata2) {
        Log::Error(fmt::format("Uncaptured device error: type {:x} ({})", static_cast<uint32_t>(type),
                               std::string(message.data, message.length)));
    };
}

void CreateDevice(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();
    auto &settings = core.GetResource<Resource::GraphicSettings>();

    SetupDeviceDescriptor(context.deviceContext.GetDescriptor(), settings);

    context.RequestDevice();

    if (!context.deviceContext.GetDevice())
        throw Exception::DeviceCreationError("Failed to create WebGPU device");
}
} // namespace Plugin::Graphic::System
