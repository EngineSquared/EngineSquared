#include "Logger.hpp"
#include "core/Core.hpp"
#include "exception/MissingResourceError.hpp"
#include "exception/MissingSchedulerError.hpp"
#include "system/WrappedSystem.hpp"

namespace Engine {

template <typename TResource> inline TResource &Core::RegisterResource(TResource &&resource)
{
    return this->_registry->ctx().insert_or_assign<TResource>(std::forward<TResource>(resource));
}

template <typename TResource> inline TResource &Core::GetResource()
{
    if (!this->_registry->ctx().contains<TResource>())
    {
        throw Exception::MissingResourceError(
            fmt::format("Resource not found in the core registry: {}", typeid(TResource).name()));
    }
    return this->_registry->ctx().get<TResource>();
}

template <typename TResource> inline bool Core::HasResource() const
{
    return this->_registry->ctx().contains<TResource>();
}

template <typename TResource> inline void Core::DeleteResource() { this->_registry->ctx().erase<TResource>(); }

template <typename TResource> inline const TResource &Core::GetResource() const
{
    return this->_registry->ctx().get<TResource>();
}

template <CScheduler TScheduler, typename... Args> inline TScheduler &Core::RegisterScheduler(Args &&...args)
{
    this->_schedulingContext.AddScheduler<TScheduler>(*this, std::forward<Args>(args)...);
    return this->_schedulingContext.GetScheduler<TScheduler>();
}

template <CScheduler TScheduler> void Core::DeleteScheduler()
{
    this->_schedulersToDelete.push_back(std::type_index(typeid(TScheduler)));
}

template <CScheduler TScheduler> inline TScheduler &Core::GetScheduler()
{
    return this->_schedulingContext.GetScheduler<TScheduler>();
}

inline Scheduler::AScheduler &Core::GetScheduler(std::type_index id)
{
    if (!this->_schedulingContext.Contains(id))
    {
        throw Exception::MissingSchedulerError(fmt::format("Scheduler not found in the core: {}", id.name()));
    }
    return *(this->_schedulingContext.GetScheduler(id));
}

template <CScheduler TScheduler, typename... Systems> inline decltype(auto) Core::RegisterSystem(Systems... systems)
{
    return this->_schedulingContext.GetScheduler<TScheduler>().AddSystems(systems...);
}

template <typename TSchedulerA, typename TSchedulerB> void Core::SetSchedulerBefore()
{
    this->_schedulingContext.Before<TSchedulerA, TSchedulerB>();
}

template <typename TSchedulerA, typename TSchedulerB> void Core::SetSchedulerAfter()
{
    this->_schedulingContext.After<TSchedulerA, TSchedulerB>();
}

template <typename TSchedulerA, typename TSchedulerB> void Core::RemoveDependencyAfter()
{
    this->_schedulingContext.RemoveDependencyAfter<TSchedulerA, TSchedulerB>();
}

template <typename TSchedulerA, typename TSchedulerB> void Core::RemoveDependencyBefore()
{
    this->_schedulingContext.RemoveDependencyBefore<TSchedulerA, TSchedulerB>();
}

template <typename... Systems> inline decltype(auto) Core::RegisterSystem(Systems... systems)
{
    if (!this->_schedulingContext.Contains(_defaultScheduler))
    {
        Log::Warning(fmt::format("Trying to register systems with a default scheduler that does not exist: {}",
                                 _defaultScheduler.name()));
    }
    return this->_schedulingContext.GetScheduler(_defaultScheduler)->AddSystems(systems...);
}

template <CScheduler TScheduler, typename System, typename ErrorCallback>
inline decltype(auto) Core::RegisterSystemWithErrorHandler(System system, ErrorCallback callback)
{
    return this->RegisterSystem<TScheduler>(WrappedSystem(system, callback));
}

template <typename System, typename ErrorCallback>
inline decltype(auto) Core::RegisterSystemWithErrorHandler(System system, ErrorCallback callback)
{
    return this->RegisterSystem(WrappedSystem(system, callback));
}

template <CPlugin... TPlugins> void Core::AddPlugins() { (AddPlugin<TPlugins>(), ...); }

template <CPlugin TPlugin> void Core::AddPlugin()
{
    if (this->_plugins.Contains(std::type_index(typeid(TPlugin))))
    {
        Log::Warning(fmt::format("Plugin {} already added", typeid(TPlugin).name()));
    }
    this->_plugins.Add<TPlugin>(*this);
    std::optional<std::shared_ptr<Engine::IPlugin>> pluginOpt = this->_plugins.Get<TPlugin>();
    if (!pluginOpt.has_value())
    {
        Log::Error("Plugin don't exist in plugin container");
        return;
    }
    auto plugin = pluginOpt.value();
    plugin->Bind();
}

template <CPlugin TPlugin> std::optional<std::shared_ptr<IPlugin>> Core::GetPlugin(void)
{
    return this->_plugins.Get<TPlugin>();
}
inline std::optional<std::shared_ptr<IPlugin>> Core::GetPlugin(std::type_index type)
{
    return this->_plugins.Get(type);
}

template <CPlugin TPlugin> bool Core::HasPlugin() const { return this->_plugins.Contains<TPlugin>(); }

template <CScheduler TScheduler> void Core::SetDefaultScheduler()
{
    SetDefaultScheduler(std::type_index(typeid(TScheduler)));
}

inline void Core::SetDefaultScheduler(std::type_index scheduler)
{
    if (!this->_schedulingContext.Contains(scheduler))
    {
        Log::Warning(fmt::format("Trying to set a default scheduler that does not exist: {}", scheduler.name()));
    }
    this->_defaultScheduler = scheduler;
}
} // namespace Engine
