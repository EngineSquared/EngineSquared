#include "plugin/APlugin.hpp"

template <typename System> inline FunctionUtils::FunctionID GetId(System system)
{
    if constexpr (Engine::SystemContainer::is_derived_from_function_type<System>::value)
    {
        return system.GetID();
    }
    else
    {
        return FunctionUtils::CallableFunction<System, void, Engine::Core &>::GetCallableID(system);
    }
}
inline FunctionUtils::FunctionID GetId(const std::unique_ptr<FunctionUtils::BaseFunction<void, Engine::Core &>> &system)
{
    return system->GetID();
}

namespace Engine {
template <CScheduler TScheduler, typename... Systems> decltype(auto) APlugin::RegisterSystems(Systems... systems)
{
    const auto schedulerCategory = GetCategory<TScheduler>();

    if (!this->_systems.contains(schedulerCategory))
    {
        std::map<FunctionUtils::FunctionID, std::type_index> temporary;
        this->_systems.insert({schedulerCategory, temporary});
    }
    auto &filteredSystems = this->_systems.at(schedulerCategory);

    _registerers.emplace_back([&filteredSystems, schedulerCategory,
                               systems = std::tuple(std::forward<Systems>(systems)...)](Engine::Core &core) mutable {
        std::apply(
            [&filteredSystems, schedulerCategory, &core](auto &&...system) {
                auto systemIds = core.RegisterSystem<TScheduler>(std::forward<decltype(system)>(system)...);
                std::apply(
                    [&filteredSystems, schedulerCategory](auto &&...args) {
                        ((filteredSystems.insert_or_assign(args, std::type_index(typeid(TScheduler)))), ...);
                    },
                    systemIds);
            },
            std::move(systems));
    });
    std::array<FunctionUtils::FunctionID, sizeof...(Systems)> temp{System<Systems>::GetCallableID(systems)...};
    return std::tuple_cat(temp);
}

template <typename TResource> void APlugin::RegisterResource(TResource &&resource)
{
    _resourceDeleters.emplace_back([](Engine::Core &core) {
        if (core.HasResource<TResource>())
            core.DeleteResource<TResource>();
        else
            Log::Error("Trying to remove a non-existing resource");
    });
    _core.RegisterResource(std::forward<TResource>(resource));
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
