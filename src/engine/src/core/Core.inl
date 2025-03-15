#include "Core.hpp"

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
    this->_schedulers.GetScheduler<TScheduler>().AddSystems<Systems...>(systems...);
    // this->_systems[std::type_index(typeid(TScheduler))].AddSystems<Systems...>(systems...);
}
} // namespace ES::Engine
