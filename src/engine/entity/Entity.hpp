#pragma once

#include <entt/entt.hpp>
#include "Registry.hpp"

namespace ES::Engine
{
    class Entity {
    public:
        Entity(int64_t entity = -1)
            : _entity(entity)
        { }

        Entity(entt::entity entity)
            : _entity(static_cast<int64_t>(entity)) // TODO: check if this is safe
        { }


        // TODO: maybe add implementation in a separate file
        operator entt::entity() const { return static_cast<entt::entity>(_entity); }
        operator int64_t() const { return _entity; }

        template <typename TComponent>
        TComponent& AddComponent(Registry& registry, TComponent&& component)
        {
            return registry.GetRegistry().emplace<TComponent>(*this, component);
        }

        template <typename TComponent, typename ...TArgs>
        TComponent& AddComponent(Registry& registry, TArgs &&...args)
        {
            return registry.GetRegistry().emplace<TComponent>(*this, std::forward<TArgs>(args)...);
        }

        template <typename ...TComponent>
        bool HasComponent(Registry& registry)
        {
            // TODO: check if this is optimal
            return registry.GetRegistry().all_of<TComponent...>(*this);
        }

        ~Entity() = default;
    private:
        // TODO: add a way to check if entity is valid or not (without using neg values for invalid entities)
        int64_t _entity;
    };
} // namespace ES::Engine
