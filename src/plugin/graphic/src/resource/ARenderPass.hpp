#pragma once

#include "core/Core.hpp"
#include "resource/Shader.hpp"
#include "utils/IValidable.hpp"
#include <glm/vec4.hpp>

namespace Graphic::Resource {

struct ColorOutput {
    std::optional<std::string> textureViewName = std::nullopt;
    std::optional<std::string> textureResolveTargetName = std::nullopt;
    uint32_t depthSlice;
    wgpu::LoadOp loadOp;
    wgpu::StoreOp storeOp;
    std::function<bool(Engine::Core &, glm::vec4 &)> getClearColorCallback;
};

struct DepthOutput {
    std::string textureName;
};

struct OutputContainer {
    std::unordered_map<uint32_t, wgpu::RenderPassColorAttachment> colorBuffers;
    std::optional<wgpu::RenderPassDepthStencilAttachment> depthBuffer;
};

struct InputContainer : public std::map<uint32_t /* index inside shader */, std::string /* bind group name */> {
    using std::map<uint32_t, std::string>::map;
};

template <typename TDerived> class ARenderPass : public Utils::IValidable {
  public:
    ARenderPass(std::string_view name) : _name(name) {}

    virtual void Execute(Engine::Core &core) = 0;

    TDerived &BindShader(std::string_view shaderName)
    {
        _boundShader = shaderName;
        return static_cast<TDerived &>(*this);
    }

    TDerived &SetGetClearColorCallback(std::function<bool(Engine::Core &, glm::vec4 &)> callback)
    {
        _getClearColorCallback = callback;
        return static_cast<TDerived &>(*this);
    }

    TDerived &AddInput(uint32_t groupIndex, std::string_view bindGroupName)
    {
        if (_inputs.contains(groupIndex))
        {
            Log::Warn(fmt::format("RenderPass {}: Overwriting existing bind group at index {}", _name, groupIndex));
        }
        _inputs[groupIndex] = bindGroupName;
        return static_cast<TDerived &>(*this);
    }

    TDerived &AddOutput(uint32_t id, wgpu::RenderPassColorAttachment output)
    {
        if (_outputs.colorBuffers.contains(id))
        {
            Log::Warn(fmt::format("RenderPass {}: Overwriting existing color buffer at index {}", _name, id));
        }
        _outputs.colorBuffers[id] = output;
        return static_cast<TDerived &>(*this);
    }

    TDerived &AddOutput(wgpu::RenderPassDepthStencilAttachment output)
    {
        if (_outputs.depthBuffer.has_value())
        {
            Log::Warn(fmt::format("RenderPass {}: Overwriting existing depth buffer", _name));
        }
        _outputs.depthBuffer = output;
        return static_cast<TDerived &>(*this);
    }

    virtual std::vector<Utils::ValidationError> validate() const override
    {
        std::vector<Utils::ValidationError> errors;
        const std::string location = fmt::format("RenderPass({})", _name);

        if (!_boundShader.has_value())
        {
            errors.push_back(Utils::ValidationError{.message = "No shader bound to render pass",
                                                    .location = location,
                                                    .severity = Utils::ValidationError::Severity::Error});
        }
        if (!_getClearColorCallback.has_value())
        {
            errors.push_back(Utils::ValidationError{
                .message = "No clear color callback set for render pass, using default clear color (black)",
                .location = location,
                .severity = Utils::ValidationError::Severity::Warning});
        }

        /**
         * TODO: Check if shader exists in resource manager
         * TODO: Check if all inputs match shader bind groups
         * TODO: Check if inputs exists in resource manager
         */

        return errors;
    };

    const auto &GetGetClearColorCallback(void) const { return _getClearColorCallback; }
    const auto &GetBoundShader(void) const { return _boundShader; }
    const auto &GetInputs(void) const { return _inputs; }
    const auto &GetName(void) const { return _name; }
    const auto &GetOutputs(void) const { return _outputs; }

  private:
    std::optional<std::function<bool(Engine::Core &, glm::vec4 &)>> _getClearColorCallback = std::nullopt;
    std::optional<std::string> _boundShader = std::nullopt;
    InputContainer _inputs;
    std::string _name;
    OutputContainer _outputs;
};
} // namespace Graphic::Resource
