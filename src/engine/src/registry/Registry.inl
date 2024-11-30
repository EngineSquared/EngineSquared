#include "Registry.hpp"

namespace ES::Engine
{
    template <typename TResource>
    inline TResource& Registry::RegisterResource(TResource&& resource)
    {
        return this->_registry->ctx().emplace<TResource>(std::forward<TResource>(resource));
    }

    template <typename TResource>
    inline TResource& Registry::GetResource()
    {
        return this->_registry->ctx().get<TResource>();
    }
} // namespace ES::Engine
