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
    this->_schedulers[std::type_index(typeid(TScheduler))] = std::make_unique<TScheduler>(*this, std::forward<Args>(args)...);
    return *static_cast<TScheduler *>(this->_schedulers[std::type_index(typeid(TScheduler))].get());
}

template <typename TScheduler> void Core::DeleteScheduler()
{
    this->_schedulersToDelete.push_back(std::type_index(typeid(TScheduler)));
}

template <typename TScheduler> inline TScheduler &Core::GetScheduler()
{
    return *static_cast<TScheduler *>(this->_schedulers[std::type_index(typeid(TScheduler))].get());
}

template <typename TScheduler, typename... Systems>
inline void Core::RegisterSystem(Systems... systems)
{
    this->_systems[std::type_index(typeid(TScheduler))].push_back([systems...](Core &registry) { (systems(registry), ...); });
}
} // namespace ES::Engine
