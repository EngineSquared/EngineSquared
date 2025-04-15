#include "APlugin.hpp"
#include "Core.hpp"
#include "Logger.hpp"

namespace ES::Engine {

template <typename TResource> inline TResource &Core::RegisterResource(TResource &&resource)
{
    return this->_registry->ctx().emplace<TResource>(std::forward<TResource>(resource));
}

template <typename TResource> inline TResource &Core::GetResource() { return this->_registry->ctx().get<TResource>(); }

template <typename TScheduler, typename... Args> inline TScheduler &Core::RegisterScheduler(Args &&...args)
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
requires std::derived_from<TScheduler, Scheduler::AScheduler>
inline void Core::RegisterSystem(Systems... systems)
{
    this->_schedulers.GetScheduler<TScheduler>().AddSystems(systems...);
}

template <typename... Systems> inline void Core::RegisterSystem(Systems... systems)
{
    this->_schedulers.GetScheduler(_defaultScheduler)->AddSystems(systems...);
}

template <typename... TPlugins> void Core::AddPlugins() { (AddPlugin<TPlugins>(), ...); }

template <typename TPlugin> void Core::AddPlugin()
{
#ifdef ES_DEBUG
    if (this->_plugins.contains(std::type_index(typeid(TPlugin))))
    {
        ES::Utils::Log::Warn(fmt::format("Plugin {} already added", typeid(TPlugin).name()));
    }
#endif
    this->_plugins.emplace(std::type_index(typeid(TPlugin)), std::make_unique<TPlugin>(*this));
    this->_plugins.at(std::type_index(typeid(TPlugin)))->Bind();
}

template <typename TPlugin> bool Core::HasPlugin() const
{
    return this->_plugins.contains(std::type_index(typeid(TPlugin)));
}

inline void Core::SetDefaultScheduler(std::type_index scheduler)
{
#ifdef ES_DEBUG
    if (!this->_schedulers.Contains(scheduler))
    {
        ES::Utils::Log::Warn(
            fmt::format("Trying to set a default scheduler that does not exist: {}", scheduler.name()));
    }
#endif
    this->_defaultScheduler = scheduler;
}

} // namespace ES::Engine
