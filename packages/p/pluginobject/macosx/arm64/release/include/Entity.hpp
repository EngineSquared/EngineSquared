#pragma once

#include "Core.hpp"
#include "EntityToIDString.hpp"
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
     * @param   core    registry used to store the entity
     * @return  new entity
     */
    static Entity Create(Core &core)
    {
        Entity entity = core.CreateEntity();
        ES::Utils::Log::Debug(
            fmt::format("[EntityID:{}] Create Entity", ES::Utils::Log::EntityToDebugString(entity_id_type(entity))));
        return entity;
    }

    /**
     * Destroy an entity in the registry.
     *
     * @param   core    registry used to store the entity
     * @return  void
     */
    void Destroy(Core &core)
    {
        ES::Utils::Log::Debug(
            fmt::format("[EntityID:{}] Destroy Entity", ES::Utils::Log::EntityToDebugString(_entity)));
        core.KillEntity(*this);
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
     * @param   core        registry used to store the component
     * @param   component   rvalue to add to registry
     * @return  reference of the added component
     */

    template <typename TComponent> inline decltype(auto) AddComponent(Core &core, TComponent &&component)
    {
        ES::Utils::Log::Debug(fmt::format("[EntityID:{}] AddComponent: {}",
                                          ES::Utils::Log::EntityToDebugString(_entity), typeid(TComponent).name()));
        return core.GetRegistry().emplace<TComponent>(ToEnttEntity(this->_entity), std::forward<TComponent>(component));
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
    template <typename TComponent, typename... TArgs> inline decltype(auto) AddComponent(Core &core, TArgs &&...args)
    {
        ES::Utils::Log::Debug(fmt::format("[EntityID:{}] AddComponent: {}",
                                          ES::Utils::Log::EntityToDebugString(_entity), typeid(TComponent).name()));
        return core.GetRegistry().emplace<TComponent>(ToEnttEntity(this->_entity), std::forward<TArgs>(args)...);
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
    inline decltype(auto) AddComponentIfNotExists(Core &core, TArgs &&...args)
    {
        if (this->HasComponents<TComponent>(core))
        {
            return this->GetComponents<TComponent>(core);
        }
        return this->AddComponent<TComponent>(core, std::forward<TArgs>(args)...);
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
    inline decltype(auto) AddTemporaryComponent(Core &core, TArgs &&...args)
    {
        if (!temporaryComponent.contains(std::type_index(typeid(TTempComponent))))
        {
            temporaryComponent[std::type_index(typeid(TTempComponent))] = [](Core &c) {
                ES::Utils::Log::Debug(fmt::format("RemoveTemporaryComponent: {}", typeid(TTempComponent).name()));
                c.GetRegistry().clear<TTempComponent>();
            };
        }

        return this->AddComponent<TTempComponent>(core, std::forward<TArgs>(args)...);
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

    /**
     * Utility method to remove a component from an entity.
     *
     * @tparam  TComponent  type to remove from registry
     * @param   core        registry used to store the component
     */
    template <typename TComponent> inline void RemoveComponent(Core &core)
    {
        ES::Utils::Log::Debug(fmt::format("[EntityID:{}] RemoveComponent: {}",
                                          ES::Utils::Log::EntityToDebugString(_entity), typeid(TComponent).name()));
        core.GetRegistry().remove<TComponent>(ToEnttEntity(this->_entity));
    }

    /**
     * Check if entity have one or multiple component's type.
     *
     * @tparam  TComponent  components to check
     * @return  true if entity have all requested component
     */
    template <typename... TComponent> inline bool HasComponents(Core &core) const
    {
        return core.GetRegistry().all_of<TComponent...>(ToEnttEntity(this->_entity));
    }

    /**
     * Get components of type TComponent from the entity.
     *
     * @tparam  TComponent  components to get
     * @return  components of type TComponent from the entity
     */
    template <typename... TComponent> inline decltype(auto) GetComponents(Core &core)
    {
        return core.GetRegistry().get<TComponent...>(ToEnttEntity(this->_entity));
    }

    /**
     * Try to get a component of type TComponent from the entity.
     *
     * @tparam  TComponent  components to get
     * @return  components of type TComponent from the entity
     */
    template <typename TComponent> inline decltype(auto) TryGetComponent(Core &core)
    {
        return core.GetRegistry().try_get<TComponent>(ToEnttEntity(this->_entity));
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
