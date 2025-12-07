#pragma once

#include "exception/FailToCreateCommandEncoderError.hpp"
#include "exception/MissingOutputRenderPassError.hpp"
#include "resource/ARenderPass.hpp"

namespace Graphic::Resource {

template <typename TDerived> class ASingleExecutionRenderPass : public ARenderPass<TDerived> {
  public:
    explicit ASingleExecutionRenderPass(std::string_view name) : ARenderPass<TDerived>(name) {}

    void Execute(Engine::Core &core) override
    {
        Resource::Context &context = core.GetResource<Resource::Context>();

        if (this->GetOutputs().colorBuffers.empty() && !this->GetOutputs().depthBuffer.has_value())
        {
            throw Exception::MissingOutputRenderPassError(
                fmt::format("RenderPass {}: No outputs defined for render pass, cannot execute.", this->GetName()));
        }

        wgpu::RenderPassEncoder renderPass = this->_CreateRenderPass(context.deviceContext, core);

        auto &shader = core.GetResource<Graphic::Resource::ShaderContainer>().Get(this->GetBoundShader().value());
        renderPass.setPipeline(shader.GetPipeline());

        for (auto &[index, name] : this->GetInputs())
        {
            Resource::BindGroup &bindGroup =
                core.GetResource<Resource::BindGroupManager>().Get(entt::hashed_string{name.c_str()});
            renderPass.setBindGroup(index, bindGroup.GetBindGroup(), 0, nullptr);
        }

        UniqueRenderCallback(renderPass, core);

        renderPass.end();
        renderPass.release();

        wgpu::CommandBufferDescriptor cmdBufferDescriptor(wgpu::Default);
        std::string cmdBufferDescriptorLabel = fmt::format("CreateRenderPass::{}::CommandBuffer", this->GetName());
        cmdBufferDescriptor.label = wgpu::StringView(cmdBufferDescriptorLabel);
        auto commandBuffer = _commandEncoder.finish(cmdBufferDescriptor);
        _commandEncoder.release();

        context.queue.value().submit(1, &commandBuffer);
        commandBuffer.release();
    }

    virtual void UniqueRenderCallback(wgpu::RenderPassEncoder &renderPass, Engine::Core &core) = 0;

  private:
    wgpu::RenderPassEncoder _CreateRenderPass(Resource::DeviceContext &context, Engine::Core &core)
    {
        auto &device = context.GetDevice().value();

        wgpu::CommandEncoderDescriptor encoderDesc(wgpu::Default);
        std::string encoderDescLabel = fmt::format("CreateRenderPass::{}::CommandEncoder", this->GetName());
        encoderDesc.label = wgpu::StringView(encoderDescLabel);
        _commandEncoder = device.createCommandEncoder(encoderDesc);
        if (_commandEncoder == nullptr)
            throw Exception::FailToCreateCommandEncoderError(fmt::format(
                "CreateRenderPass::{}::Command encoder is not created, cannot draw sprite.", this->GetName()));

        wgpu::RenderPassDescriptor renderPassDesc(wgpu::Default);
        std::string renderPassDescLabel = fmt::format("CreateRenderPass::{}::RenderPass", this->GetName());
        renderPassDesc.label = wgpu::StringView(renderPassDescLabel);

        std::vector<wgpu::RenderPassColorAttachment> colorAttachments;
        colorAttachments.reserve(this->GetOutputs().colorBuffers.size());

        for (const auto &colorTextureName : this->GetOutputs().colorBuffers)
        {
            const auto &colorTexture = colorTextureName.second;
            wgpu::RenderPassColorAttachment colorAttachment(wgpu::Default);
            std::string textureViewName = colorTexture.textureViewName;
            auto textureView = core.GetResource<Resource::TextureContainer>()
                                   .Get(entt::hashed_string{textureViewName.c_str()})
                                   .GetDefaultView();
            colorAttachment.view = textureView;
            if (colorTexture.textureResolveTargetName.has_value())
            {
                std::string resolveTargetName = colorTexture.textureResolveTargetName.value();
                auto resolveTextureView = core.GetResource<Resource::TextureContainer>()
                                              .Get(entt::hashed_string{resolveTargetName.c_str()})
                                              .GetDefaultView();
                colorAttachment.resolveTarget = resolveTextureView;
            }
            colorAttachment.storeOp = colorTexture.storeOp;
            glm::vec4 clearColor(0.0f);
            if (colorTexture.getClearColorCallback(core, clearColor))
            {
                colorAttachment.clearValue = wgpu::Color{clearColor.r, clearColor.g, clearColor.b, clearColor.a};
                colorAttachment.loadOp = wgpu::LoadOp::Clear;
            }
            else
            {
                colorAttachment.loadOp = wgpu::LoadOp::Load;
            }
            colorAttachments.push_back(colorAttachment);
        }

        renderPassDesc.colorAttachmentCount = static_cast<uint32_t>(colorAttachments.size());
        renderPassDesc.colorAttachments = colorAttachments.data();

        if (this->GetOutputs().depthBuffer.has_value())
        {
            renderPassDesc.depthStencilAttachment = &this->GetOutputs().depthBuffer.value();
        }

        return _commandEncoder.beginRenderPass(renderPassDesc);
    }

    wgpu::CommandEncoder _commandEncoder;
};
} // namespace Graphic::Resource
