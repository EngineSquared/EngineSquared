#pragma once

#include "core/Core.hpp"
#include "resource/BindGroupManager.hpp"
#include "resource/Shader.hpp"
#include "resource/ShaderContainer.hpp"
#include "utils/IValidable.hpp"
#include <glm/vec4.hpp>

namespace Graphic::Resource {

struct ColorOutput {
    entt::hashed_string textureId{};
    explicit ColorOutput(std::string_view textureId_ = {})
    {
        if (!textureId_.empty())
        {
            textureId = entt::hashed_string(textureId_.data(), textureId_.size());
        }
    }

    std::optional<std::string> textureResolveTargetName = std::nullopt;
    uint32_t depthSlice = 0;
    wgpu::StoreOp storeOp = wgpu::StoreOp::Store;
    std::function<bool(Engine::Core &, glm::vec4 &)> getClearColorCallback = [](Engine::Core &, glm::vec4 &) {
        return false;
    };
};

struct DepthOutput {
    std::string textureName;
};

struct OutputContainer {
    std::unordered_map<uint32_t, ColorOutput> colorBuffers;
    std::optional<wgpu::RenderPassDepthStencilAttachment> depthBuffer;
};

struct InputContainer : public std::map<uint32_t /* index inside shader */, std::string /* bind group name */> {
    using std::map<uint32_t, std::string>::map;
};

class ARenderPass {
  public:
    explicit ARenderPass(std::string_view name) : _name(name) {}

    virtual void Execute(Engine::Core &core) = 0;

    void BindShader(std::string_view shaderName)
    {
        _boundShader = entt::hashed_string(shaderName.data(), shaderName.size());
    }

    void AddInput(uint32_t groupIndex, std::string_view bindGroupName)
    {
        if (_inputs.contains(groupIndex))
        {
            Log::Warn(fmt::format("RenderPass {}: Overwriting existing bind group at index {}", _name, groupIndex));
        }
        _inputs[groupIndex] = bindGroupName;
    }

    void AddOutput(uint32_t id, ColorOutput &&output)
    {
        if (_outputs.colorBuffers.contains(id))
        {
            Log::Warn(fmt::format("RenderPass {}: Overwriting existing color buffer at index {}", _name, id));
        }
        _outputs.colorBuffers[id] = std::move(output);
    }

    void AddOutput(wgpu::RenderPassDepthStencilAttachment output)
    {
        if (_outputs.depthBuffer.has_value())
        {
            Log::Warn(fmt::format("RenderPass {}: Overwriting existing depth buffer", _name));
        }
        _outputs.depthBuffer = output;
    }

    std::vector<Utils::ValidationError> validate(Engine::Core &core) const
    {
        std::vector<Utils::ValidationError> errors;
        const std::string location = fmt::format("RenderPass({})", _name);
        const auto &shaderManager = core.GetResource<Resource::ShaderContainer>();

        if (!_boundShader.has_value())
        {
            errors.push_back(Utils::ValidationError{.message = "No shader bound to render pass",
                                                    .location = location,
                                                    .severity = Utils::ValidationError::Severity::Error});
        }
        else
        {
            if (!shaderManager.Contains(_boundShader.value()))
            {
                errors.push_back(Utils::ValidationError{
                    .message = fmt::format("Bound shader '{}' does not exist in ShaderManager",
                                           std::string_view(_boundShader->data(), _boundShader->size())),
                    .location = location,
                    .severity = Utils::ValidationError::Severity::Error});
            }
            const auto &shader = shaderManager.Get(_boundShader.value());
            for (const auto &[index, bindGroupName] : _inputs)
            {
                const auto &bindGroupLayouts = shader.GetDescriptor().getBindGroupLayouts();
                if (index >= bindGroupLayouts.size())
                {
                    errors.push_back(Utils::ValidationError{
                        .message = fmt::format(
                            "Input bind group index {} exceeds number of bind groups ({}) in shader '{}'", index,
                            bindGroupLayouts.size(), std::string_view(_boundShader->data(), _boundShader->size())),
                        .location = location,
                        .severity = Utils::ValidationError::Severity::Error});
                    continue;
                }
            }
        }
        const auto &bindGroups = core.GetResource<Resource::BindGroupManager>();
        for (const auto &[index, bindGroupName] : _inputs)
        {
            if (!bindGroups.Contains(entt::hashed_string(bindGroupName.data(), bindGroupName.size())))
            {
                errors.push_back(Utils::ValidationError{
                    .message = fmt::format("Input bind group '{}' at index {} does not exist in BindGroupContainer",
                                           bindGroupName, index),
                    .location = location,
                    .severity = Utils::ValidationError::Severity::Error});
            }
        }
        return errors;
    };

    const auto &GetBoundShader(void) const { return _boundShader; }
    const auto &GetInputs(void) const { return _inputs; }
    const auto &GetName(void) const { return _name; }
    const auto &GetOutputs(void) const { return _outputs; }

  private:
    std::optional<entt::hashed_string> _boundShader = std::nullopt;
    InputContainer _inputs;
    std::string _name;
    OutputContainer _outputs;
};
} // namespace Graphic::Resource
