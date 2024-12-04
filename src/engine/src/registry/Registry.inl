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

template <typename TScheduler> inline TScheduler &Registry::RegisterScheduler()
{
    this->_schedulers[std::type_index(typeid(TScheduler))] = std::make_unique<TScheduler>(*this);
    return *static_cast<TScheduler *>(this->_schedulers[std::type_index(typeid(TScheduler))].get());
}

template <typename TScheduler> inline TScheduler &Registry::GetScheduler()
{
    return *static_cast<TScheduler *>(this->_schedulers[std::type_index(typeid(TScheduler))].get());
}

template <typename TScheduler> inline void Registry::RegisterSystem(USystem const &f)
{
    this->_systems[std::type_index(typeid(TScheduler))].push_back(f);
}
} // namespace ES::Engine
