#include "system/initialization/CreateDevice.hpp"
#include "exception/DeviceCreationError.hpp"
#include "resource/DeviceContext.hpp"
#include "resource/Adapter.hpp"
#include "resource/GraphicSettings.hpp"

namespace Graphic::System {
static void SetupDeviceDescriptor(wgpu::DeviceDescriptor &deviceDesc, Graphic::Resource::GraphicSettings &settings)
{
    deviceDesc.label = wgpu::StringView("Core Device");
    deviceDesc.requiredFeatureCount = settings.GetRequiredFeatures().size();
    deviceDesc.requiredFeatures = settings.GetRequiredFeatures().data();
    deviceDesc.requiredLimits = dynamic_cast<wgpu::Limits *>(&settings.GetWantedLimits());
    deviceDesc.defaultQueue.nextInChain = nullptr;
    deviceDesc.defaultQueue.label = wgpu::StringView("The default queue");
    deviceDesc.deviceLostCallbackInfo.nextInChain = nullptr;
    deviceDesc.deviceLostCallbackInfo.mode = wgpu::CallbackMode::AllowProcessEvents;
    deviceDesc.deviceLostCallbackInfo.callback = [](WGPUDevice const *device, WGPUDeviceLostReason reason,
                                                    WGPUStringView message, WGPU_NULLABLE void *userdata1,
                                                    WGPU_NULLABLE void *userdata2) {
        Log::Error(fmt::format("Device lost: reason {:x} ({})", static_cast<uint32_t>(reason),
                               std::string(message.data, message.length)));
    };
    deviceDesc.uncapturedErrorCallbackInfo.nextInChain = nullptr;
    deviceDesc.uncapturedErrorCallbackInfo.callback = settings.GetOnErrorCallback();
}

void CreateDevice(Engine::Core &core)
{
    auto &deviceContext = core.GetResource<Resource::DeviceContext>();
    auto &adapter = core.GetResource<Resource::Adapter>();
    auto &settings = core.GetResource<Resource::GraphicSettings>();

    SetupDeviceDescriptor(deviceContext.GetDescriptor(), settings);

    deviceContext.GetDevice() = adapter->requestDevice(deviceContext.GetDescriptor());

    if (!deviceContext.GetDevice())
        throw Exception::DeviceCreationError("Failed to create WebGPU device");
}
} // namespace Graphic::System
