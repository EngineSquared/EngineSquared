#pragma once

#include "resource/ARenderPass.hpp"

namespace Graphic::Resource {

class SingleExecutionRenderPass : public ARenderPass<SingleExecutionRenderPass> {
  public:
    SingleExecutionRenderPass(std::string_view name) : ARenderPass(name) {}

    void Execute(Engine::Core &core) override
    {
        Resource::DeviceContext &deviceContext = core.GetResource<Resource::DeviceContext>();

        wgpu::RenderPassEncoder renderPass = this->_CreateRenderPass(deviceContext);

        for (auto &[index, name] : this->GetInputs())
        {
            Resource::BindGroup &bindGroup = core.GetResource<Resource::BindGroupManager>().Get(name);
            renderPass.setBindGroup(index, bindGroup.GetBindGroup(), 0, nullptr);
        }
    }

  private:
    wgpu::RenderPassEncoder _CreateRenderPass(Resource::DeviceContext &context)
    {
        auto &device = context.GetDevice().value();

        wgpu::CommandEncoderDescriptor encoderDesc(wgpu::Default);
        std::string encoderDescLabel = fmt::format("CreateRenderPass::{}::CommandEncoder", this->GetName());
        encoderDesc.label = wgpu::StringView(encoderDescLabel);
        auto commandEncoder = device.createCommandEncoder(encoderDesc);
        if (commandEncoder == nullptr)
            throw std::runtime_error(fmt::format(
                "CreateRenderPass::{}::Command encoder is not created, cannot draw sprite.", this->GetName()));

        std::vector<wgpu::CommandBuffer> commandBuffers;

        wgpu::RenderPassDescriptor renderPassDesc(wgpu::Default);
        std::string renderPassDescLabel = fmt::format("CreateRenderPass::{}::RenderPass", this->GetName());
        renderPassDesc.label = wgpu::StringView(renderPassDescLabel);

        std::vector<wgpu::RenderPassColorAttachment> colorAttachments;
        colorAttachments.reserve(this->GetOutputs().colorBuffers.size());

        for (const auto &colorTextureName : this->GetOutputs().colorBuffers)
        {
            colorAttachments.push_back(colorTextureName.second);
        }

        renderPassDesc.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
        renderPassDesc.colorAttachments = colorAttachments.data();

        if (this->GetOutputs().depthBuffer.has_value())
        {
            renderPassDesc.depthStencilAttachment = &this->GetOutputs().depthBuffer.value();
        }

        return commandEncoder.beginRenderPass(renderPassDesc);
    }

    std::optional<std::function<void(wgpu::RenderPassEncoder &renderPass, Engine::Core &core)>> uniqueRenderCallback =
        std::nullopt;
};
} // namespace Graphic::Resource
