#include "Logger.hpp"
#include "fmt/format.h"

#include "resource/Context.hpp"
#include "resource/Shader.hpp"
#include "resource/ShaderDescriptor.hpp"

#include "spdlog/fmt/bundled/format.h"

#include "utils/IValidable.hpp"
#include "utils/RmluiRenderPass.hpp"
#include "utils/shader/BindGroupLayout.hpp"
#include "utils/shader/BufferBindGroupLayoutEntry.hpp"
#include "utils/shader/ColorTargetState.hpp"
#include "utils/shader/SamplerBindGroupLayoutEntry.hpp"
#include "utils/shader/TextureBindGroupLayoutEntry.hpp"
#include "utils/shader/VertexBufferLayout.hpp"
#include "utils/webgpu.hpp"

#include <RmlUi/Core/Vertex.h>
#include <cstddef>

Graphic::Resource::Shader Rmlui::Utils::RmluiRenderPass::CreateShader(Graphic::Resource::Context &graphicContext)
{
    Graphic::Resource::ShaderDescriptor shaderDescriptor;

    auto vertexLayout = Graphic::Utils::VertexBufferLayout()
                            .addVertexAttribute(wgpu::VertexFormat::Float32x2, offsetof(Rml::Vertex, position), 0)
                            .addVertexAttribute(wgpu::VertexFormat::Uint8x4, offsetof(Rml::Vertex, colour), 1)
                            .addVertexAttribute(wgpu::VertexFormat::Float32x2, offsetof(Rml::Vertex, tex_coord), 2)
                            .setArrayStride(sizeof(Rml::Vertex))
                            .setStepMode(wgpu::VertexStepMode::Vertex);

    auto textureLayout = Graphic::Utils::BindGroupLayout("RmluiTextureLayout")
                             .addEntry(Graphic::Utils::TextureBindGroupLayoutEntry("uiTexture")
                                           .setSampleType(wgpu::TextureSampleType::Float)
                                           .setViewDimension(wgpu::TextureViewDimension::_2D)
                                           .setVisibility(wgpu::ShaderStage::Fragment)
                                           .setBinding(0))
                             .addEntry(Graphic::Utils::SamplerBindGroupLayoutEntry("uiSampler")
                                           .setType(wgpu::SamplerBindingType::Filtering)
                                           .setVisibility(wgpu::ShaderStage::Fragment)
                                           .setBinding(1));

    auto screenLayout = Graphic::Utils::BindGroupLayout("RmluiScreenLayout")
                            .addEntry(Graphic::Utils::BufferBindGroupLayoutEntry("screen")
                                          .setType(wgpu::BufferBindingType::Uniform)
                                          .setMinBindingSize(sizeof(float) * 4)
                                          .setVisibility(wgpu::ShaderStage::Vertex)
                                          .setBinding(0));

    wgpu::BlendState blendState(wgpu::Default);
    blendState.color.srcFactor = wgpu::BlendFactor::One;
    blendState.color.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    blendState.color.operation = wgpu::BlendOperation::Add;
    blendState.alpha.srcFactor = wgpu::BlendFactor::One;
    blendState.alpha.dstFactor = wgpu::BlendFactor::OneMinusSrcAlpha;
    blendState.alpha.operation = wgpu::BlendOperation::Add;

    auto colorOutput = Graphic::Utils::ColorTargetState("END_RENDER_TEXTURE")
                           .setFormat(wgpu::TextureFormat::BGRA8UnormSrgb)
                           .setBlendState(blendState);

    shaderDescriptor.setShader(RMLUI_RENDER_PASS_SHADER_CONTENT)
        .setName(RMLUI_RENDER_PASS_SHADER_NAME)
        .setVertexEntryPoint("vs_main")
        .setFragmentEntryPoint("fs_main")
        .addBindGroupLayout(textureLayout)
        .addBindGroupLayout(screenLayout)
        .addVertexBufferLayout(vertexLayout)
        .addOutputColorFormat(colorOutput)
        .setCullMode(wgpu::CullMode::None);

    if (const auto validations = shaderDescriptor.validate(); !validations.empty())
    {
        for (const auto &validation : validations)
        {
            if (validation.severity == Graphic::Utils::ValidationError::Severity::Error)
            {
                Log::Error(fmt::format("Rmlui Shader Descriptor Validation Error: {} at {}", validation.message,
                                       validation.location));
            }
            else if (validation.severity == Graphic::Utils::ValidationError::Severity::Warning)
            {
                Log::Warn(fmt::format("Rmlui Shader Descriptor Validation Warning: {} at {}", validation.message,
                                      validation.location));
            }
        }
    }

    return Graphic::Resource::Shader::Create(shaderDescriptor, graphicContext);
}
