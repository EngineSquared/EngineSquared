#include "entity/Entity.hpp"

namespace Engine {
template <typename TComponent>
decltype(auto) Entity::AddComponent(TComponent &&component)
{
    return _entityId.AddComponent(GetCore(), std::forward<TComponent>(component));
}

template <typename TComponent, typename... TArgs>
decltype(auto) Entity::AddComponent(TArgs &&...args)
{
    return _entityId.AddComponent<TComponent>(GetCore(), std::forward<TArgs>(args)...);
}

template <typename TComponent, typename... TArgs>
decltype(auto) Entity::AddComponentIfNotExists(TArgs &&...args)
{
    return _entityId.AddComponentIfNotExists<TComponent>(GetCore(), std::forward<TArgs>(args)...);
}

template <typename TTempComponent, typename... TArgs>
decltype(auto) Entity::AddTemporaryComponent(TArgs &&...args)
{
    return _entityId.AddTemporaryComponent<TTempComponent>(GetCore(), std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::RemoveComponent()
{
    _entityId.RemoveComponent<TComponent>(GetCore());
}

template <typename... TComponent>
bool Entity::HasComponents() const
{
    return _entityId.HasComponents<TComponent...>(GetCore());
}

template <typename... TComponent>
decltype(auto) Entity::GetComponents()
{
    return _entityId.GetComponents<TComponent...>(GetCore());
}

template <typename... TComponent>
decltype(auto) Entity::GetComponents() const
{
    return _entityId.GetComponents<TComponent...>(GetCore());
}

template <typename TComponent>
decltype(auto) Entity::TryGetComponent()
{
    return _entityId.TryGetComponent<TComponent>(GetCore());
}
} // namespace Engine

template <typename FormatContext>
auto fmt::formatter<Engine::Entity>::format(const Engine::Entity &entity, FormatContext &ctx) const
{
    return fmt::formatter<Engine::EntityId>::format(entity.Id(), ctx);
}
