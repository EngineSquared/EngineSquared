#include "entity/EntityId.hpp"

namespace Engine {
constexpr EntityId::EntityId(ValueType v) : Id{v} {}

constexpr EntityId::EntityId(Id id) : Id{id} {}

constexpr EntityId::operator ValueType() const { return value; }

constexpr EntityId EntityId::Null() { return EntityId{NullValue()}; }

bool EntityId::IsValid(const Engine::Core &core) const { return core.IsEntityValid(value); }

template <typename TComponent> decltype(auto) EntityId::AddComponent(Engine::Core &core, TComponent &&component)
{
    /// @todo put the log in the core emplace
    Log::Debug(fmt::format("[EntityID:{}] AddComponent: {}", value, typeid(TComponent).name()));
    return core.GetRegistry().emplace<TComponent>(value, std::forward<TComponent>(component));
}

template <typename TComponent, typename... TArgs>
decltype(auto) EntityId::AddComponent(Engine::Core &core, TArgs &&...args)
{
    /// @todo put the log in the core emplace
    Log::Debug(fmt::format("[EntityID:{}] AddComponent: {}", value, typeid(TComponent).name()));
    return core.GetRegistry().emplace<TComponent>(value, std::forward<TArgs>(args)...);
}

template <typename TComponent, typename... TArgs>
decltype(auto) EntityId::AddComponentIfNotExists(Engine::Core &core, TArgs &&...args)
{
    if (this->HasComponents<TComponent>(core))
    {
        return this->GetComponents<TComponent>(core);
    }
    return this->AddComponent<TComponent>(core, std::forward<TArgs>(args)...);
}

template <typename... TComponent> bool EntityId::HasComponents(const Engine::Core &core) const
{
    return core.GetRegistry().all_of<TComponent...>(value);
}

template <typename TTempComponent, typename... TArgs>
decltype(auto) EntityId::AddTemporaryComponent(Engine::Core &core, TArgs &&...args)
{
    if (!temporaryComponent.contains(std::type_index(typeid(TTempComponent))))
    {
        temporaryComponent[std::type_index(typeid(TTempComponent))] = [](Core &c) {
            Log::Debug(fmt::format("RemoveTemporaryComponent: {}", typeid(TTempComponent).name()));
            c.GetRegistry().clear<TTempComponent>();
        };
    }

    return this->AddComponent<TTempComponent>(core, std::forward<TArgs>(args)...);
}

template <typename... TComponent> decltype(auto) EntityId::GetComponents(Engine::Core &core)
{
    return core.GetRegistry().get<TComponent...>(this->value);
}

template <typename... TComponent> decltype(auto) EntityId::GetComponents(Engine::Core &core) const
{
    return core.GetRegistry().get<TComponent...>(this->value);
}

template <typename TComponent> decltype(auto) EntityId::TryGetComponent(Engine::Core &core)
{
    return core.GetRegistry().try_get<TComponent>(this->value);
}

template <typename TComponent> void EntityId::RemoveComponent(Engine::Core &core)
{
    /// @todo put the log in the core emplace
    Log::Debug(fmt::format("[EntityID:{}] RemoveComponent: {}", value, typeid(TComponent).name()));
    core.GetRegistry().remove<TComponent>(value);
}

void EntityId::RemoveTemporaryComponents(Core &core)
{
    if (temporaryComponent.empty())
    {
        return;
    }
    for (const auto &[typeIndex, func] : temporaryComponent)
    {
        func(core);
    }
    temporaryComponent.clear();
}
} // namespace Engine

std::size_t std::hash<Engine::EntityId>::operator()(const Engine::EntityId &entityId) const noexcept
{
    return std::hash<entt::id_type>{}(entityId.value);
}

template <typename FormatContext>
auto fmt::formatter<Engine::EntityId>::format(const Engine::EntityId &entityId, FormatContext &ctx) const
{
    if (entityId.IsNull())
    {
        return fmt::formatter<std::string>::format("null_entity", ctx);
    }
    return fmt::formatter<std::string>::format(Log::EntityToDebugString(entityId.value), ctx);
}
