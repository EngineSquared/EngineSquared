#include "PluginContainer.hpp"

template <typename TPlugin, typename... Args>
void Engine::PluginContainer::Add(Core &core, Args &&...args)
{
    if (this->_plugins.contains(std::type_index(typeid(TPlugin))))
    {
        Log::Warning(fmt::format("Plugin already exists: {}", typeid(TPlugin).name()));
        return;
    }
    Log::Debug(fmt::format("Adding scheduler: {}", typeid(TPlugin).name()));
    std::shared_ptr<TPlugin> scheduler = std::make_shared<TPlugin>(core, std::forward<Args>(args)...);
    this->_plugins[std::type_index(typeid(TPlugin))] = scheduler;
    this->_orderedPlugins.push_back(scheduler);
}

template <typename TPlugin> inline std::optional<std::shared_ptr<Engine::IPlugin>> Engine::PluginContainer::Get()
{
    Update();
    auto it = this->_plugins.find(std::type_index(typeid(TPlugin)));
    return it->second;
}

template <typename TBefore, typename TAfter> inline void Engine::PluginContainer::Before()
{
    _dirty = true;
    _dependencies[std::type_index(typeid(TAfter))].insert(std::type_index(typeid(TBefore)));
}

template <typename TAfter, typename TBefore> inline void Engine::PluginContainer::After() { Before<TBefore, TAfter>(); }

template <typename TBefore, typename TAfter> void Engine::PluginContainer::RemoveDependencyBefore()
{
    RemoveDependencyBefore(typeid(TBefore), typeid(TAfter));
}

template <typename TAfter, typename TBefore> void Engine::PluginContainer::RemoveDependencyAfter()
{
    RemoveDependencyBefore(typeid(TBefore), typeid(TAfter));
}

inline bool Engine::PluginContainer::Contains(std::type_index id) const { return this->_plugins.contains(id); }

template <typename TPlugin> inline bool Engine::PluginContainer::Contains() const
{
    return Contains(std::type_index(typeid(TPlugin)));
}

template <typename TPlugin> inline void Engine::PluginContainer::Delete()
{
    this->Delete(std::type_index(typeid(TPlugin)));
}

inline std::optional<std::shared_ptr<Engine::IPlugin>> Engine::PluginContainer::Get(std::type_index id)
{
    Update();
    auto it = this->_plugins.find(id);
    return it->second;
}
