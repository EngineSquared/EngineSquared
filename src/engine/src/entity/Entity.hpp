#pragma once

#include "EntityToIDString.hpp"
#include "Id.hpp"
#include "Logger.hpp"
#include "core/Core.hpp"
#include "entity/EntityId.hpp"
#include <entt/entt.hpp>
#include <typeindex>

namespace Engine {
/**
 * Contains a single value which must correspond to an index in a registry.
 *
 * Should be used as a wrapper / container of an entt::entity
 */
class Entity {
  public:
    /**
     * Create a ES Entity from entity_entityId_type
     *
     * @param   core    reference to the core
     * @param   entity  index value in the registry
     */
    explicit(false) Entity(Core &core, EntityId entityId) : _core(core), _entityId(entityId) {}

    explicit Entity() : _core(std::nullopt), _entityId(EntityId::Null()) {}

    static Entity Null() { return Entity{}; }

    ~Entity() = default;

    /**
     * Check whenever if entity id is a valid id.
     * @return  entity's validity
     */
    bool IsValid() const;

    inline EntityId Id() const { return _entityId; }

    explicit(false) inline operator EntityId() const { return _entityId; }

    /**
     * Utility method to add a component to an entity.
     *
     * @tparam  TComponent  type to add to registry
     * @param   core        registry used to store the component
     * @param   component   rvalue to add to registry
     * @return  reference of the added component
     */
    template <typename TComponent> inline decltype(auto) AddComponent(TComponent &&component)
    {
        return _entityId.AddComponent(_core.value(), std::forward<TComponent>(component));
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
    template <typename TComponent, typename... TArgs> inline decltype(auto) AddComponent(TArgs &&...args)
    {
        return _entityId.AddComponent<TComponent>(_core.value(), std::forward<TArgs>(args)...);
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
    template <typename TComponent, typename... TArgs> inline decltype(auto) AddComponentIfNotExists(TArgs &&...args)
    {
        return _entityId.AddComponentIfNotExists<TComponent>(_core.value(), std::forward<TArgs>(args)...);
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
    template <typename TTempComponent, typename... TArgs> inline decltype(auto) AddTemporaryComponent(TArgs &&...args)
    {
        return _entityId.AddTemporaryComponent<TTempComponent>(_core.value(), std::forward<TArgs>(args)...);
    }

    /**
     * System to remove all temporary component from the registry.
     *
     * @param   core    registry used to store the component
     * @return  void
     * @see     AddTemporaryComponent
     */
    static inline void RemoveTemporaryComponents(Core &core) { EntityId::RemoveTemporaryComponents(core); }

    /**
     * Utility method to remove a component from an entity.
     *
     * @tparam  TComponent  type to remove from registry
     * @param   core        registry used to store the component
     */
    template <typename TComponent> inline void RemoveComponent()
    {
        _entityId.RemoveComponent<TComponent>(_core.value());
    }

    /**
     * Check if entity have one or multiple component's type.
     *
     * @tparam  TComponent  components to check
     * @return  true if entity have all requested component
     */
    template <typename... TComponent> inline bool HasComponents() const
    {
        return _entityId.HasComponents<TComponent...>(GetCore());
    }

    /**
     * Get components of type TComponent from the entity.
     *
     * @tparam  TComponent  components to get
     * @return  components of type TComponent from the entity
     */
    template <typename... TComponent> inline decltype(auto) GetComponents()
    {
        return _entityId.GetComponents<TComponent...>(GetCore());
    }

    /**
     * Get components of type TComponent from the entity.
     *
     * @tparam  TComponent  components to get
     * @return  components of type TComponent from the entity
     */
    template <typename... TComponent> inline decltype(auto) GetComponents() const
    {
        return _entityId.GetComponents<TComponent...>(GetCore());
    }

    /**
     * Try to get a component of type TComponent from the entity.
     *
     * @tparam  TComponent  components to get
     * @return  components of type TComponent from the entity
     */
    template <typename TComponent> inline decltype(auto) TryGetComponent()
    {
        return _entityId.TryGetComponent<TComponent>(GetCore());
    }

    bool operator==(const Entity &rhs) const { return _entityId.value == rhs._entityId.value; }

    bool operator==(const EntityId &rhs) const { return _entityId.value == rhs.value; }

  private:
    Core &GetCore() { return _core->get(); }

    const Core &GetCore() const { return _core->get(); }

    std::optional<std::reference_wrapper<Core>> _core;
    EntityId _entityId;
    inline static std::unordered_map<std::type_index, std::function<void(Core &)>> temporaryComponent = {};
};

} // namespace Engine

template <> struct fmt::formatter<Engine::Entity> : fmt::formatter<Engine::EntityId> {
    template <typename FormatContext> auto format(const Engine::Entity &entity, FormatContext &ctx) const
    {
        return fmt::formatter<Engine::EntityId>::format(entity.Id(), ctx);
    }
};
