#pragma once

#include "Core.hpp"
#include "Logger.hpp"
#include <entt/entt.hpp>
#include <typeindex>

namespace ES::Engine {
/**
 * Contains a single value which must correspond to an index in a registry.
 *
 * Should be used as a wrapper / container of an entt::entity
 */
class Entity {
  public:
    using entity_id_type = uint32_t;
    inline static const entity_id_type entity_null_id = entt::null;

  public:
    /**
     * Create a ES Entity from entity_id_type
     *
     * @param   entity  index value in the registry
     */
    explicit(false) Entity(entity_id_type entity = entity_null_id) : _entity(entity) {}

    /**
     * Create a ES Entity from Entt Entity
     *
     * @param   entt    index value in the registry
     */
    explicit(false) Entity(entt::entity entity) : Entity(FromEnttEntity(entity)) {}

    ~Entity() = default;

    /**
     * Create a new entity in the registry.
     *
     * @param   registry    registry used to store the entity
     * @return  new entity
     */
    static Entity Create(Core &registry)
    {
        Entity entity = registry.CreateEntity();
#ifdef DEBUG
        ES::Utils::Log::Info(fmt::format("[EntityID:{}] Create Entity", entity_id_type(entity)));
#endif
        return entity;
    }

    /**
     * Destroy an entity in the registry.
     *
     * @param   registry    registry used to store the entity
     * @return  void
     */
    void Destroy(Core &registry)
    {
#ifdef DEBUG
        ES::Utils::Log::Info(fmt::format("[EntityID:{}] Destroy Entity", _entity));
#endif
        registry.KillEntity(*this);
    }

    /**
     * Implicit cast into entt::entity.
     */
    explicit(false) operator entt::entity() const { return static_cast<entt::entity>(_entity); }

    /**
     * Explicit cast into entity_id_type.
     */
    explicit operator entity_id_type() const { return _entity; }

    /**
     * Check whenever if entity id is a valid id.
     * @return  entity's validity
     */
    bool IsValid() const;

    /**
     * Utility method to add a component to an entity.
     *
     * @tparam  TComponent  type to add to registry
     * @param   registry    registry used to store the component
     * @param   component   rvalue to add to registry
     * @return  reference of the added component
     */

    template <typename TComponent> inline decltype(auto) AddComponent(Core &registry, TComponent &&component)
    {
#ifdef DEBUG
        ES::Utils::Log::Info(fmt::format("[EntityID:{}] AddComponent: {}", _entity, typeid(TComponent).name()));
#endif
        return registry.GetRegistry().emplace<TComponent>(ToEnttEntity(this->_entity),
                                                          std::forward<TComponent>(component));
    }

    /**
     * Utility method to add a component to an entity.
     *
     * @tparam  TComponent  type to add to registry
     * @tparam  TArgs       type used to create the component
     * @param   registry    registry used to store the component
     * @param   args        parameters used to instanciate component directly in registry memory
     * @return  reference of the added component
     */
    template <typename TComponent, typename... TArgs>
    inline decltype(auto) AddComponent(Core &registry, TArgs &&...args)
    {
#ifdef DEBUG
        ES::Utils::Log::Info(fmt::format("[EntityID:{}] AddComponent: {}", _entity, typeid(TComponent).name()));
#endif
        return registry.GetRegistry().emplace<TComponent>(ToEnttEntity(this->_entity), std::forward<TArgs>(args)...);
    }

    /**
     * Utility method to add a component to an entity if it does not already exist.
     *
     * @tparam  TComponent  type to add to registry
     * @tparam  TArgs       type used to create the component
     * @param   registry    registry used to store the component
     * @param   args        parameters used to instanciate component directly in registry memory
     * @return  reference of the added component
     */
    template <typename TComponent, typename... TArgs>
    inline decltype(auto) AddComponentIfNotExists(Core &registry, TArgs &&...args)
    {
        if (this->HasComponents<TComponent>(registry))
        {
            return this->GetComponents<TComponent>(registry);
        }
        return this->AddComponent<TComponent>(registry, std::forward<TArgs>(args)...);
    }

    /**
     * Utility method to add a temporary component to an entity.
     * Temporary component are removed when calling RemoveTemporaryComponents system.
     *
     * @tparam  TTempComponent  type to add to registry
     * @tparam  TArgs           type used to create the component
     * @param   registry        registry used to store the component
     * @param   args            parameters used to instanciate component directly in registry memory
     * @return  reference of the added component
     * @see     RemoveTemporaryComponents
     */
    template <typename TTempComponent, typename... TArgs>
    inline decltype(auto) AddTemporaryComponent(Core &registry, TArgs &&...args)
    {
        if (!temporaryComponent.contains(std::type_index(typeid(TTempComponent))))
        {
            temporaryComponent[std::type_index(typeid(TTempComponent))] = [](Core &reg) {
#ifdef DEBUG
                ES::Utils::Log::Info(fmt::format("RemoveTemporaryComponent: {}", typeid(TTempComponent).name()));
#endif
                reg.GetRegistry().clear<TTempComponent>();
            };
        }

        return this->AddComponent<TTempComponent>(registry, std::forward<TArgs>(args)...);
    }

    /**
     * System to remove all temporary component from the registry.
     *
     * @param   registry    registry used to store the component
     * @return  void
     * @see     AddTemporaryComponent
     */
    static void RemoveTemporaryComponents(Core &registry)
    {
        if (temporaryComponent.empty())
        {
            return;
        }
        for (const auto &[typeIndex, func] : temporaryComponent)
        {
            func(registry);
        }
        temporaryComponent.clear();
    }

    /**
     * Utility method to remove a component from an entity.
     *
     * @tparam  TComponent  type to remove from registry
     * @param   registry    registry used to store the component
     */
    template <typename TComponent> inline void RemoveComponent(Core &registry)
    {
#ifdef DEBUG
        ES::Utils::Log::Info(fmt::format("[EntityID:{}] RemoveComponent: {}", _entity, typeid(TComponent).name()));
#endif
        registry.GetRegistry().remove<TComponent>(ToEnttEntity(this->_entity));
    }

    /**
     * Check if entity have one or multiple component's type.
     *
     * @tparam  TComponent  components to check
     * @return  true if entity have all requested component
     */
    template <typename... TComponent> inline bool HasComponents(Core &registry)
    {
        return registry.GetRegistry().all_of<TComponent...>(ToEnttEntity(this->_entity));
    }

    /**
     * Get components of type TComponent from the entity.
     *
     * @tparam  TComponent  components to get
     * @return  components of type TComponent from the entity
     */
    template <typename... TComponent> inline decltype(auto) GetComponents(Core &registry)
    {
        return registry.GetRegistry().get<TComponent...>(ToEnttEntity(this->_entity));
    }

    /**
     * Try to get a component of type TComponent from the entity.
     *
     * @tparam  TComponent  components to get
     * @return  components of type TComponent from the entity
     */
    template <typename TComponent> inline decltype(auto) TryGetComponent(Core &registry)
    {
        return registry.GetRegistry().try_get<TComponent>(ToEnttEntity(this->_entity));
    }

    inline static entity_id_type FromEnttEntity(entt::entity e) { return static_cast<entity_id_type>(e); }

    inline static entt::entity ToEnttEntity(entity_id_type e) { return static_cast<entt::entity>(e); }

    bool operator==(const Entity &rhs) const = default;

    /**
     * Compare two entities id.
     *
     * @param   rhs     entity to compare
     * @return  true if entities id are equals
     */
    inline bool operator==(const entity_id_type &rhs) const { return _entity == rhs; }

    /**
     * Compare two entities id.
     *
     * @param   rhs     entity to compare
     * @return  true if entities id are different
     */
    inline bool operator!=(const entity_id_type &rhs) const { return _entity != rhs; }

  private:
    entity_id_type _entity;
    inline static std::unordered_map<std::type_index, std::function<void(Core &)>> temporaryComponent = {};
};

} // namespace ES::Engine
