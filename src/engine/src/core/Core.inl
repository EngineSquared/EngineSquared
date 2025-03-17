#include "Core.hpp"
#include "Logger.hpp"
#include "APlugin.hpp" // Include APlugin.hpp

namespace ES::Engine {

template <typename TResource>
inline TResource& Core::RegisterResource(TResource&& resource)
{
    return this->_registry->ctx().emplace<TResource>(std::forward<TResource>(resource));
}


template <typename TResource> inline TResource &Core::GetResource()
{
    return this->_registry->ctx().get<TResource>();
}

template <typename TScheduler, typename... Args> inline TScheduler &Core::RegisterScheduler(Args &&... args)
{
    this->_schedulers.AddScheduler<TScheduler>(*this, std::forward<Args>(args)...);
    return this->_schedulers.GetScheduler<TScheduler>();
}

template <typename TScheduler> void Core::DeleteScheduler()
{
    this->_schedulersToDelete.push_back(std::type_index(typeid(TScheduler)));
}

template <typename TScheduler> inline TScheduler &Core::GetScheduler()
{
    return this->_schedulers.GetScheduler<TScheduler>();
}

template <typename TScheduler, typename... Systems>
inline void Core::RegisterSystem(Systems... systems)
{
    this->_schedulers.GetScheduler<TScheduler>().AddSystems(systems...);
}

template <typename... TPlugins>
void Core::AddPlugins()
{
    (AddPlugin<TPlugins>(), ...);
}

template <typename TPlugin>
void Core::AddPlugin()
{
    #ifdef DEBUG
    if (this->_plugins.contains(std::type_index(typeid(TPlugin)))) {
        ES::Utils::Log::Warn(fmt::format("Plugin {} already added", typeid(TPlugin).name()));
    }
    #endif
    this->_plugins.emplace(std::type_index(typeid(TPlugin)), std::make_unique<TPlugin>(*this));
    this->_plugins.at(std::type_index(typeid(TPlugin)))->Build();
}

template <typename TPlugin>
bool Core::HasPlugin() const
{
    return this->_plugins.contains(std::type_index(typeid(TPlugin)));
}
} // namespace ES::Engine
