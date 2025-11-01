#pragma once

#include "core/Core.hpp"
#include "resource/Shader.hpp"
#include <glm/vec4.hpp>
#include "utils/IValidable.hpp"
#include "core/Core.hpp"

namespace Graphic::Resource {

template <typename TDerived>
class ARenderPass : public Utils::IValidable {
  public:
    ARenderPass(std::string_view name) : _name(name) {}

    virtual TDerived &Execute(Engine::Core &core) = 0;

    const std::string &GetName() const { return _name; }

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

    virtual std::vector<Utils::ValidationError> validate() const override {
        std::vector<Utils::ValidationError> errors;
        const std::string location = fmt::format("RenderPass({})", _name);

        if (!_boundShader.has_value()) {
            errors.push_back(Utils::ValidationError{
                .message = "No shader bound to render pass",
                .location = location,
                .severity = Utils::ValidationError::Severity::Error
            });
        }
        if (!_getClearColorCallback.has_value()) {
            errors.push_back(Utils::ValidationError{
                .message = "No clear color callback set for render pass, using default clear color (black)",
                .location = location,
                .severity = Utils::ValidationError::Severity::Warning
            });
        }

        /**
         * TODO: Check if shader exists in resource manager
         * TODO: Check if all inputs match shader bind groups
         * TODO: Check if inputs exists in resource manager
         */

        return errors;
    };

  private:
    std::optional<std::function<bool(Engine::Core &, glm::vec4 &)>> _getClearColorCallback;
    std::optional<std::string> _boundShader = std::nullopt;
    std::map<uint32_t /* index inside shader */, std::string /* bind group name */> _inputs; // TODO: put this in a dedicated container
    std::string _name;
};
} // namespace Graphic::Resource
