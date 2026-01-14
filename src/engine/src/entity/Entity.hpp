#pragma once

#include "EntityToIDString.hpp"
#include "Logger.hpp"
#include "core/Core.hpp"
#include <entt/entt.hpp>
#include "Id.hpp"
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
     * Create a ES Entity from entity_id_type
     *
     * @param   core    reference to the core
     * @param   entity  index value in the registry
     */
    explicit(false) Entity(Core &core, EntityId entity = EntityId::Null()) : _core(core), _entity(entity) {}

    ~Entity() = default;

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

    template <typename TComponent> inline decltype(auto) AddComponent(TComponent &&component)
    {
        // TODO: put this into core or custom registry
        Log::Debug(fmt::format("[EntityID:{}] AddComponent: {}", _entity, typeid(TComponent).name()));
        return _core.GetRegistry().emplace<TComponent>(this->_entity, std::forward<TComponent>(component));
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
        Log::Debug(fmt::format("[EntityID:{}] AddComponent: {}", _entity, typeid(TComponent).name()));
        return _core.GetRegistry().emplace<TComponent>(this->_entity, std::forward<TArgs>(args)...);
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
    inline decltype(auto) AddComponentIfNotExists(TArgs &&...args)
    {
        if (this->HasComponents<TComponent>())
        {
            return this->GetComponents<TComponent>();
        }
        return this->AddComponent<TComponent>(std::forward<TArgs>(args)...);
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
    inline decltype(auto) AddTemporaryComponent(TArgs &&...args)
    {
        if (!temporaryComponent.contains(std::type_index(typeid(TTempComponent))))
        {
            temporaryComponent[std::type_index(typeid(TTempComponent))] = [](Core &c) {
                Log::Debug(fmt::format("RemoveTemporaryComponent: {}", typeid(TTempComponent).name()));
                c.GetRegistry().clear<TTempComponent>();
            };
        }

        return this->AddComponent<TTempComponent>(std::forward<TArgs>(args)...);
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
    template <typename TComponent> inline void RemoveComponent()
    {
        Log::Debug(fmt::format("[EntityID:{}] RemoveComponent: {}", _entity,
                               typeid(TComponent).name()));
        _core.GetRegistry().remove<TComponent>(this->_entity);
    }

    /**
     * Check if entity have one or multiple component's type.
     *
     * @tparam  TComponent  components to check
     * @return  true if entity have all requested component
     */
    template <typename... TComponent> inline bool HasComponents() const
    {
        return _core.GetRegistry().all_of<TComponent...>(this->_entity);
    }

    /**
     * Get components of type TComponent from the entity.
     *
     * @tparam  TComponent  components to get
     * @return  components of type TComponent from the entity
     */
    template <typename... TComponent> inline decltype(auto) GetComponents()
    {
        return _core.GetRegistry().get<TComponent...>(this->_entity);
    }

    /**
     * Get components of type TComponent from the entity.
     *
     * @tparam  TComponent  components to get
     * @return  components of type TComponent from the entity
     */
    template <typename... TComponent> inline decltype(auto) GetComponents() const
    {
        return _core.GetRegistry().get<TComponent...>(this->_entity);
    }

    /**
     * Try to get a component of type TComponent from the entity.
     *
     * @tparam  TComponent  components to get
     * @return  components of type TComponent from the entity
     */
    template <typename TComponent> inline decltype(auto) TryGetComponent()
    {
        return _core.GetRegistry().try_get<TComponent>(this->_entity);
    }

    bool operator==(const Entity &rhs) const {
        return _entity.value == rhs._entity.value;
    }

  private:
    Core &_core;
    EntityId _entity;
    inline static std::unordered_map<std::type_index, std::function<void(Core &)>> temporaryComponent = {};
};

} // namespace Engine
