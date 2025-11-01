#pragma once

#include "core/Core.hpp"
#include "resource/Shader.hpp"
#include <glm/vec4.hpp>

namespace Graphic::Resource {

template <typename TDerived> class ARenderPass {
  public:
    ARenderPass(std::string_view name) : _name(name) {}

    const std::string &GetName() const { return _name; }

    TDerived &BindShader(std::weak_ptr<Shader> shader)
    {
        _boundShader = shader;
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

    virtual TDerived &Execute(Engine::Core &core) = 0;

  private:
    std::function<bool(Engine::Core &, glm::vec4 &)> _getClearColorCallback;
    std::weak_ptr<Shader> _boundShader;
    std::map<uint32_t /* index inside shader */, std::string /* bind group name */>
        _inputs; // TODO: put this in a dedicated container
    std::string _name;
};
} // namespace Graphic::Resource
