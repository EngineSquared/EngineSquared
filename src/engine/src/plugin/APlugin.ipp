#include "plugin/APlugin.hpp"

namespace Engine {
template <CScheduler TScheduler, typename... Systems> decltype(auto) APlugin::RegisterSystems(Systems... systems)
{
    auto systemIds = _core.RegisterSystem<TScheduler>(systems...);
    const auto schedulerCategory = GetCategory<TScheduler>();

    if (!this->_systems.contains(schedulerCategory))
    {
        std::map<FunctionUtils::FunctionID, std::type_index> temporary;
        this->_systems.insert({schedulerCategory, temporary});
    }
    auto &filteredSystems = this->_systems.at(schedulerCategory);

    std::apply([&filteredSystems, schedulerCategory](
                   auto &&...args) { ((filteredSystems.insert_or_assign(args, typeid(TScheduler))), ...); },
               systemIds);
    return systemIds;
}

template <typename TResource> TResource &APlugin::RegisterResource(TResource &&resource)
{
    _resourceDeleters.emplace_back([](Engine::Core &core) {
        if (core.HasResource<TResource>())
            core.DeleteResource<TResource>();
        else
            Log::Error("Trying to remove a non-existing resource");
    });
    return _core.RegisterResource(std::forward<TResource>(resource));
}

template <CPlugin... TPlugins> void APlugin::RequirePlugins() { (RequirePlugin<TPlugins>(), ...); }

template <CScheduler TScheduler, typename... Args> TScheduler &APlugin::RegisterScheduler(Args &&...args)
{
    return _core.RegisterScheduler<TScheduler>(std::forward<Args>(args)...);
}

template <CPlugin TPlugin> void APlugin::RequirePlugin()
{
    if (!_core.HasPlugin<TPlugin>())
    {
        _core.AddPlugins<TPlugin>();
    }
    _requiredPluginIds.insert(typeid(TPlugin));
    _core.GetPlugins().Before(typeid(TPlugin), typeid(*this));
}
} // namespace Engine
