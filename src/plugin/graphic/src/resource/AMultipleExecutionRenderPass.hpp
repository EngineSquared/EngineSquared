#pragma once

#include "exception/FailToCreateCommandEncoderError.hpp"
#include "exception/MissingOutputRenderPassError.hpp"
#include "resource/ARenderPass.hpp"

namespace Graphic::Resource {

template <typename TDerived> class AMultipleExecutionRenderPass : public ARenderPass {
  public:
    explicit AMultipleExecutionRenderPass(std::string_view name) : ARenderPass(name) {}

    virtual uint16_t GetNumberOfPasses(Engine::Core &core) = 0;

    virtual void preMultiplePass(Engine::Core &core) {
        // Default implementation does nothing
    };
    virtual void postMultiplePass(Engine::Core &core) {
        // Default implementation does nothing
    };
    virtual void perPass(uint16_t passIndex, Engine::Core &core) {
        // Default implementation does nothing
    };
    virtual void postPass(uint16_t passIndex, Engine::Core &core) {
        // Default implementation does nothing
    };

    void Execute(Engine::Core &core) override
    {
        preMultiplePass(core);
        const uint16_t numberOfPasses = GetNumberOfPasses(core);
        for (uint16_t passIndex = 0; passIndex < numberOfPasses; passIndex++)
        {
            perPass(passIndex, core);
            ExecuteSinglePass(core);
            postPass(passIndex, core);
        }
        postMultiplePass(core);
    }

    void ExecuteSinglePass(Engine::Core &core)
    {
        Resource::Context &context = core.GetResource<Resource::Context>();

        if (this->GetOutputs().colorBuffers.empty() && !this->GetOutputs().depthBuffer.has_value())
        {
            Log::Error(
                fmt::format("RenderPass {}: No outputs defined for render pass, cannot execute.", this->GetName()));
            return;
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

            entt::hashed_string textureId = colorTexture.textureId;
            auto textureView = core.GetResource<Resource::TextureContainer>().Get(textureId).GetDefaultView();

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

        wgpu::RenderPassDepthStencilAttachment depthAttachment(wgpu::Default);
        if (this->GetOutputs().depthBuffer.has_value())
        {
            const auto &depthTexture = this->GetOutputs().depthBuffer.value();

            wgpu::TextureView depthView;
            if (depthTexture.depthTextureView.has_value())
            {
                depthView = depthTexture.depthTextureView.value();
            }
            else
            {
                entt::hashed_string textureId = depthTexture.textureId;
                depthView = core.GetResource<Resource::TextureContainer>().Get(textureId).GetDefaultView();
            }

            depthAttachment.view = depthView;
            depthAttachment.depthStoreOp = depthTexture.storeOp;
            float clearDepth = 1.0f;
            if (depthTexture.getClearDepthCallback(core, clearDepth))
            {
                depthAttachment.depthClearValue = clearDepth;
                depthAttachment.depthLoadOp = wgpu::LoadOp::Clear;
            }
            else
            {
                depthAttachment.depthLoadOp = wgpu::LoadOp::Load;
            }
            renderPassDesc.depthStencilAttachment = &depthAttachment;
        }

        return _commandEncoder.beginRenderPass(renderPassDesc);
    }

    wgpu::CommandEncoder _commandEncoder;
};
} // namespace Graphic::Resource
