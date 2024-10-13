#include "Entity.hpp"

namespace ES::Engine
{
    template <typename TComponent>
    inline TComponent& Entity::AddComponent(Registry& registry, TComponent&& component)
    {
        return registry.GetRegistry().emplace<TComponent>(ToEnttEntity(this->_entity), component);
    }

    template <typename TComponent, typename ...TArgs>
    inline TComponent& Entity::AddComponent(Registry& registry, TArgs &&...args)
    {
        return registry.GetRegistry().emplace<TComponent>(ToEnttEntity(this->_entity), std::forward<TArgs>(args)...);
    }

    template <typename ...TComponent>
    inline bool Entity::HasComponents(Registry& registry)
    {
        return registry.GetRegistry().all_of<TComponent...>(ToEnttEntity(this->_entity));
    }
}