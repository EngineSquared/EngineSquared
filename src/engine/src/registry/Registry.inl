#include "Registry.hpp"

namespace ES::Engine {

template <typename TResource>
inline TResource& Registry::RegisterResource(TResource&& resource)
{
    return this->_registry->ctx().emplace<TResource>(std::forward<TResource>(resource));
}


template <typename TResource> inline TResource &Registry::GetResource()
{
    return this->_registry->ctx().get<TResource>();
}

template <typename TScheduler, typename... Args> inline TScheduler &Registry::RegisterScheduler(Args &&... args)
{
    this->_schedulers[std::type_index(typeid(TScheduler))] = std::make_unique<TScheduler>(*this, std::forward<Args>(args)...);
    return *static_cast<TScheduler *>(this->_schedulers[std::type_index(typeid(TScheduler))].get());
}

template <typename TScheduler> void Registry::DeleteScheduler()
{
    this->_schedulersToDelete.push_back(std::type_index(typeid(TScheduler)));
}

template <typename TScheduler> inline TScheduler &Registry::GetScheduler()
{
    return *static_cast<TScheduler *>(this->_schedulers[std::type_index(typeid(TScheduler))].get());
}

template <typename TScheduler> inline void Registry::RegisterSystem(USystem const &f)
{
    this->_systems[std::type_index(typeid(TScheduler))].push_back(f);
}

template <typename TScheduler, typename... Systems>
inline void Registry::RegisterSystem(Systems... systems)
{
    this->_systems[std::type_index(typeid(TScheduler))].push_back([systems...](Registry &registry) { (systems(registry), ...); });
}
} // namespace ES::Engine
