#pragma once

#include "Id.hpp"
#include <fmt/format.h>

namespace Engine {

struct EntityId : Id {
    using ValueType = Id::ValueType;
    constexpr explicit(false) EntityId(ValueType v = NullValue()) : Id{v} {}
    constexpr explicit(false) EntityId(Id id) : Id{id} {}

    constexpr operator ValueType() const { return value; }
    static constexpr EntityId Null() { return EntityId{NullValue()}; }

    /**
     * Utility method to add a component to an entity.
     *
     * @tparam  TComponent  type to add to registry
     * @param   core        registry used to store the component
     * @param   component   rvalue to add to registry
     * @return  reference of the added component
     */
    template <typename TComponent> inline decltype(auto) AddComponent(Engine::Core &core, TComponent &&component)
    {
        // TODO: put this into core or custom registry
        Log::Debug(fmt::format("[EntityID:{}] AddComponent: {}", value, typeid(TComponent).name()));
        return core.GetRegistry().emplace<TComponent>(value, std::forward<TComponent>(component));
    }

    /**
     * Utility method to add a component to an entity.
     *
     * @tparam  TComponent  type to add to registry
     * @tparam  TArgs       type used to create the component
     * @param   core        registry used to store the component
     * @param   args        parameters used to instanciate component directly in registry memory
     * @return  reference of the added component
     */
    template <typename TComponent, typename... TArgs>
    inline decltype(auto) AddComponent(Engine::Core &core, TArgs &&...args)
    {
        Log::Debug(fmt::format("[EntityID:{}] AddComponent: {}", value, typeid(TComponent).name()));
        return core.GetRegistry().emplace<TComponent>(value, std::forward<TArgs>(args)...);
    }

    /**
     * Utility method to add a component to an entity if it does not already exist.
     *
     * @tparam  TComponent  type to add to registry
     * @tparam  TArgs       type used to create the component
     * @param   core        registry used to store the component
     * @param   args        parameters used to instanciate component directly in registry memory
     * @return  reference of the added component
     */
    template <typename TComponent, typename... TArgs>
    inline decltype(auto) AddComponentIfNotExists(Engine::Core &core, TArgs &&...args)
    {
        if (this->HasComponents<TComponent>(core))
        {
            return this->GetComponents<TComponent>(core);
        }
        return this->AddComponent<TComponent>(core, std::forward<TArgs>(args)...);
    }

    /**
     * Check if entity have one or multiple component's type.
     *
     * @tparam  TComponent  components to check
     * @return  true if entity have all requested component
     */
    template <typename... TComponent> inline bool HasComponents(const Engine::Core &core) const
    {
        return core.GetRegistry().all_of<TComponent...>(value);
    }

    /**
     * Utility method to add a temporary component to an entity.
     * Temporary component are removed when calling RemoveTemporaryComponents system.
     *
     * @tparam  TTempComponent  type to add to registry
     * @tparam  TArgs           type used to create the component
     * @param   core            registry used to store the component
     * @param   args            parameters used to instanciate component directly in registry memory
     * @return  reference of the added component
     * @see     RemoveTemporaryComponents
     */
    template <typename TTempComponent, typename... TArgs>
    inline decltype(auto) AddTemporaryComponent(Engine::Core &core, TArgs &&...args)
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

    /**
     * Get components of type TComponent from the entity.
     *
     * @tparam  TComponent  components to get
     * @return  components of type TComponent from the entity
     */
    template <typename... TComponent> inline decltype(auto) GetComponents(Engine::Core &core)
    {
        return core.GetRegistry().get<TComponent...>(this->value);
    }

    /**
     * Get components of type TComponent from the entity.
     *
     * @tparam  TComponent  components to get
     * @return  components of type TComponent from the entity
     */
    template <typename... TComponent> inline decltype(auto) GetComponents(Engine::Core &core) const
    {
        return core.GetRegistry().get<TComponent...>(this->value);
    }

    /**
     * Try to get a component of type TComponent from the entity.
     *
     * @tparam  TComponent  components to get
     * @return  components of type TComponent from the entity
     */
    template <typename TComponent> inline decltype(auto) TryGetComponent(Engine::Core &core)
    {
        return core.GetRegistry().try_get<TComponent>(this->value);
    }

    /**
     * Utility method to remove a component from an entity.
     *
     * @tparam  TComponent  type to remove from registry
     * @param   core        registry used to store the component
     */
    template <typename TComponent> inline void RemoveComponent(Engine::Core &core)
    {
        Log::Debug(fmt::format("[EntityID:{}] RemoveComponent: {}", value, typeid(TComponent).name()));
        core.GetRegistry().remove<TComponent>(value);
    }

    /**
     * System to remove all temporary component from the registry.
     *
     * @param   core    registry used to store the component
     * @return  void
     * @see     AddTemporaryComponent
     */
    static void RemoveTemporaryComponents(Core &core)
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

  private:
    inline static std::unordered_map<std::type_index, std::function<void(Core &)>> temporaryComponent = {};
};

static_assert(sizeof(EntityId) == sizeof(EntityId::ValueType), "EntityId size must be equal to Id size");
} // namespace Engine

template <> struct fmt::formatter<Engine::EntityId> : fmt::formatter<std::string> {
    template <typename FormatContext> auto format(const Engine::EntityId &entityId, FormatContext &ctx) const
    {
        if (entityId.IsNull())
        {
            return fmt::formatter<std::string>::format("null_entity", ctx);
        }
        return fmt::formatter<std::string>::format(Log::EntityToDebugString(entityId.value), ctx);
    }
};
