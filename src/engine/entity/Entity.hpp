#pragma once

#include <entt/entt.hpp>
#include "Registry.hpp"

namespace ES::Engine
{
    class Entity {
    public:
        using entity_id_type = uint32_t;
        static const entity_id_type entity_null_id = entt::null;
    public:
        Entity(entity_id_type entity = -1)
            : _entity(entity)
        { }

        Entity(entt::entity entity)
            : _entity(FromEnttEntity(entity))
        { }
        ~Entity() = default;

        operator entt::entity() const { return static_cast<entt::entity>(_entity); }
        operator entity_id_type() const { return _entity; }

        template <typename TComponent>
        TComponent& AddComponent(Registry& registry, TComponent&& component)
        {
            return registry.GetRegistry().emplace<TComponent>(ToEnttEntity(this->_entity), component);
        }

        template <typename TComponent, typename ...TArgs>
        TComponent& AddComponent(Registry& registry, TArgs &&...args)
        {
            return registry.GetRegistry().emplace<TComponent>(ToEnttEntity(this->_entity), std::forward<TArgs>(args)...);
        }

        template <typename ...TComponent>
        bool HasComponents(Registry& registry)
        {
            return registry.GetRegistry().all_of<TComponent...>(ToEnttEntity(this->_entity));
        }

        bool IsValid()
        {
            return _entity != entity_null_id;
        }

        static Entity CreateNullEntity()
        {
            return Entity(entt::entity(entt::null));
        }
    private:
        static entity_id_type FromEnttEntity(entt::entity e)
        {
            return static_cast<entity_id_type>(e);
        }

        static entt::entity ToEnttEntity(entity_id_type e)
        {
            return static_cast<entt::entity>(e);
        }
    private:
        entity_id_type _entity;
    };
} // namespace ES::Engine
