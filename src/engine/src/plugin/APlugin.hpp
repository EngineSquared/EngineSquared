#pragma once

#include "Core.hpp"

namespace ES::Engine {
class APlugin {
  public:
    explicit APlugin(Core &core) : _core(core){};
    virtual void Bind(void) = 0;

    // AddSystem
    template <typename TScheduler, typename... Systems> void RegisterSystems(Systems... systems)
    {
        _core.RegisterSystem<TScheduler>(systems...);
    }

    template <typename TResource> TResource &RegisterResource(TResource &&resource)
    {
        return _core.RegisterResource(std::forward<TResource>(resource));
    }

    template <typename... TPlugins> void RequirePlugins() { (RequirePlugin<TPlugins>(), ...); }

  private:
    template <typename TPlugin> void RequirePlugin()
    {
        if (!_core.HasPlugin<TPlugin>())
        {
            _core.AddPlugins<TPlugin>();
        }
    }

    Core &_core;
};
} // namespace ES::Engine
