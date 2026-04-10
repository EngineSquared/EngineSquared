#include "plugin/APlugin.hpp"

namespace Engine {
template <CScheduler TScheduler, typename... Systems>
decltype(auto) APlugin::RegisterSystems(Systems... systems)
{
    return _core.RegisterSystem<TScheduler>(systems...);
}

template <typename TResource>
TResource &APlugin::RegisterResource(TResource &&resource)
{
    return _core.RegisterResource(std::forward<TResource>(resource));
}

template <CPlugin... TPlugins>
void APlugin::RequirePlugins()
{
    (RequirePlugin<TPlugins>(), ...);
}

template <CScheduler TScheduler, typename... Args>
TScheduler &APlugin::RegisterScheduler(Args &&...args)
{
    return _core.RegisterScheduler<TScheduler>(std::forward<Args>(args)...);
}

template <CPlugin TPlugin>
void APlugin::RequirePlugin()
{
    if (!_core.HasPlugin<TPlugin>())
    {
        _core.AddPlugins<TPlugin>();
    }
}
} // namespace Engine
