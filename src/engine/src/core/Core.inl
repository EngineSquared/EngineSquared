#include "Logger.hpp"
#include "core/Core.hpp"
#include "exception/MissingResourceError.hpp"
#include "system/WrappedSystem.hpp"

namespace Engine {

template <typename TResource> inline TResource &Core::RegisterResource(TResource &&resource)
{
    return this->_registry->ctx().emplace<TResource>(std::forward<TResource>(resource));
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

template <typename TResource> inline void Core::DeleteResource() { this->_registry->ctx().erase<TResource>(); }

template <CScheduler TScheduler, typename... Args> inline TScheduler &Core::RegisterScheduler(Args &&...args)
{
    this->_schedulers.AddScheduler<TScheduler>(*this, std::forward<Args>(args)...);
    return this->_schedulers.GetScheduler<TScheduler>();
}

template <CScheduler TScheduler> void Core::DeleteScheduler()
{
    this->_schedulersToDelete.push_back(std::type_index(typeid(TScheduler)));
}

template <CScheduler TScheduler> inline TScheduler &Core::GetScheduler()
{
    return this->_schedulers.GetScheduler<TScheduler>();
}

template <CScheduler TScheduler, typename... Systems> inline decltype(auto) Core::RegisterSystem(Systems... systems)
{
    return this->_schedulers.GetScheduler<TScheduler>().AddSystems(systems...);
}

template <typename... Systems> inline decltype(auto) Core::RegisterSystem(Systems... systems)
{
    if (!this->_schedulers.Contains(_defaultScheduler))
    {
        Log::Warn(fmt::format("Trying to register systems with a default scheduler that does not exist: {}",
                              _defaultScheduler.name()));
    }
    return this->_schedulers.GetScheduler(_defaultScheduler)->AddSystems(systems...);
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

template <typename... TPlugins> void Core::AddPlugins() { (AddPlugin<TPlugins>(), ...); }

template <typename TPlugin> void Core::AddPlugin()
{
    if (this->_plugins.contains(std::type_index(typeid(TPlugin))))
    {
        Log::Warn(fmt::format("Plugin {} already added", typeid(TPlugin).name()));
    }
    this->_plugins.emplace(std::type_index(typeid(TPlugin)), std::make_unique<TPlugin>(*this));
    this->_plugins.at(std::type_index(typeid(TPlugin)))->Bind();
}

template <typename TPlugin> bool Core::HasPlugin() const
{
    return this->_plugins.contains(std::type_index(typeid(TPlugin)));
}

inline void Core::SetDefaultScheduler(std::type_index scheduler)
{
    if (!this->_schedulers.Contains(scheduler))
    {
        Log::Warn(fmt::format("Trying to set a default scheduler that does not exist: {}", scheduler.name()));
    }
    this->_defaultScheduler = scheduler;
}
} // namespace Engine
