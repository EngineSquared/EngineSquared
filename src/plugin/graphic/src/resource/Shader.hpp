#pragma once

#include "resource/ShaderDescriptor.hpp"
#include "utils/BindGroupLayout.hpp"
#include "utils/ColorTargetState.hpp"
#include "utils/DepthStencilState.hpp"
#include "utils/VertexBufferLayout.hpp"
#include "utils/webgpu.hpp"
#include <vector>

namespace Graphic::Resource {

class Shader {
  public:
    Shader(void) = default;
    virtual ~Shader() = default;

    static Shader Create(const ShaderDescriptor &descriptor, Context &context)
    {
        wgpu::Device device = context.deviceContext.GetDevice().value();
        Shader shader;
        shader.descriptor = descriptor;

        const std::string &name = descriptor.getName();

        wgpu::ShaderModule shaderModule = _createShaderModule(name, descriptor.getShaderSource().value(), device);

        wgpu::RenderPipelineDescriptor pipelineDescriptor(wgpu::Default);
        const std::string pipelineLabel = fmt::format("{} Render Pipeline", name);
        pipelineDescriptor.label = wgpu::StringView(pipelineLabel);

        std::vector<wgpu::VertexBufferLayout> vertexBufferLayouts = _createVertexBufferLayouts(descriptor);
        pipelineDescriptor.vertex.bufferCount = vertexBufferLayouts.size();
        pipelineDescriptor.vertex.buffers = vertexBufferLayouts.data();
        pipelineDescriptor.vertex.module = shaderModule;
        pipelineDescriptor.vertex.entryPoint = wgpu::StringView(descriptor.getVertexEntryPoint());

        wgpu::FragmentState fragmentState(wgpu::Default);
        std::vector<wgpu::ColorTargetState> colorTargets(descriptor.getOutputColorFormats().size());
        wgpu::BlendState blendState(wgpu::Default);
        for (const auto &format : descriptor.getOutputColorFormats())
        {
            wgpu::ColorTargetState colorTarget(wgpu::Default);
            colorTarget.format = format.getFormat();
            colorTarget.writeMask = wgpu::ColorWriteMask::All;
            colorTarget.blend = &blendState;
            colorTargets.push_back(colorTarget);
        }
        fragmentState.module = shaderModule;
        fragmentState.entryPoint = wgpu::StringView(descriptor.getFragmentEntryPoint());
        fragmentState.targetCount = colorTargets.size();
        fragmentState.targets = colorTargets.data();
        pipelineDescriptor.fragment = &fragmentState;

        wgpu::PipelineLayoutDescriptor pipelineLayoutDescriptor(wgpu::Default);
        const std::string pipelineLayoutLabel = fmt::format("{} Pipeline Layout", name);
        pipelineLayoutDescriptor.label = wgpu::StringView(pipelineLayoutLabel);
        std::vector<WGPUBindGroupLayout> bindGroupLayouts;
        for (const auto &layout : descriptor.getBindGroupLayouts())
        {
            std::vector<WGPUBindGroupLayoutEntry> entries;
            for (const auto &entry : layout.getEntries())
            {
                entries.push_back(entry->getEntry());
            }
            wgpu::BindGroupLayoutDescriptor bindGroupLayoutDescriptor(wgpu::Default);
            const std::string layoutLabel = fmt::format("{} Bind Group Layout of pipeline {}", layout.getName(), name);
            bindGroupLayoutDescriptor.label = wgpu::StringView(layoutLabel);
            bindGroupLayoutDescriptor.entryCount = entries.size();
            bindGroupLayoutDescriptor.entries = entries.data();
            bindGroupLayouts.push_back(device.createBindGroupLayout(bindGroupLayoutDescriptor));
        }
        pipelineLayoutDescriptor.bindGroupLayoutCount = bindGroupLayouts.size();
        pipelineLayoutDescriptor.bindGroupLayouts = bindGroupLayouts.data();
        pipelineDescriptor.layout = device.createPipelineLayout(pipelineLayoutDescriptor);

        pipelineDescriptor.depthStencil = &descriptor.getOutputDepthFormat()->getValue();

        pipelineDescriptor.primitive.topology = descriptor.getPrimitiveTopology();
        pipelineDescriptor.primitive.cullMode = descriptor.getCullMode();

        shader.pipeline = device.createRenderPipeline(pipelineDescriptor);

        return shader;
    }

    const ShaderDescriptor &getDescriptor() const { return descriptor; }

  private:
    static wgpu::ShaderModule _createShaderModule(std::string_view name, std::string_view source,
                                                  const wgpu::Device &device)
    {
        wgpu::ShaderSourceWGSL wgslDesc(wgpu::Default);
        wgslDesc.code = wgpu::StringView(source);
        wgpu::ShaderModuleDescriptor shaderModuleDesc(wgpu::Default);
        shaderModuleDesc.nextInChain = &wgslDesc.chain;
        const std::string label = fmt::format("{} Shader Module", name);
        shaderModuleDesc.label = wgpu::StringView(label);
        return device.createShaderModule(shaderModuleDesc);
    }

    static std::vector<wgpu::VertexBufferLayout> _createVertexBufferLayouts(const ShaderDescriptor &descriptor)
    {
        std::vector<wgpu::VertexBufferLayout> layouts;
        for (const auto &layout : descriptor.getVertexBufferLayouts())
        {
            wgpu::VertexBufferLayout vertexBufferLayout(wgpu::Default);
            vertexBufferLayout.arrayStride = layout.getArrayStride();
            vertexBufferLayout.stepMode = layout.getStepMode();
            vertexBufferLayout.attributeCount = layout.getVertexAttributes().size();
            vertexBufferLayout.attributes = layout.getVertexAttributes().data();
            layouts.push_back(vertexBufferLayout);
        }
        return layouts;
    }

    ShaderDescriptor descriptor;
    wgpu::RenderPipeline pipeline;
};

} // namespace Graphic::Resource
