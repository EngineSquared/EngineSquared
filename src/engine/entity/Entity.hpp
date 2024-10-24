#pragma once

#include "Registry.hpp"
#include <entt/entt.hpp>

namespace ES::Engine {
/**
 * Contains a single value which must correspond to an index in a registry.
 *
 * Should be used as a wrapper / container of an entt::entity
 */
class Entity {
  public:
    using entity_id_type = uint32_t;
    static const entity_id_type entity_null_id = entt::null;

  public:
    /**
     * Create a ES Entity from Entt Entity
     *
     * @param   entt    index value in the registry
     */
    Entity(entt::entity entity = entt::null) : _entity(FromEnttEntity(entity)) {}
    ~Entity() = default;

    /**
     * Explicit cast into entt::entity.
     */
    operator entt::entity() const { return static_cast<entt::entity>(_entity); }

    /**
     * Explicit cast into entity_id_type.
     */
    operator entity_id_type() const { return _entity; }

    /**
     * Check whenever if entity id is a valid id.
     * @return  entity's validity
     */
    bool IsValid();

    /**
     * Utility method to add a component to an entity.
     *
     * @tparam  TComponent  type to add to registry
     * @param   registry    registry used to store the component
     * @param   component   rvalue to add to registry
     * @return  reference of the added component
     */

    template <typename TComponent> inline decltype(auto) AddComponent(Registry &registry, TComponent &&component)
    {
        return registry.GetRegistry().emplace<TComponent>(ToEnttEntity(this->_entity), component);
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
    inline decltype(auto) AddComponent(Registry &registry, TArgs &&...args)
    {
        return registry.GetRegistry().emplace<TComponent>(ToEnttEntity(this->_entity), std::forward<TArgs>(args)...);
    }

    /**
     * Check if entity have one or multiple component's type.
     *
     * @tparam  TComponent  components to check
     * @return  true if entity have all requested component
     */
    template <typename... TComponent> inline bool HasComponents(Registry &registry)
    {
        return registry.GetRegistry().all_of<TComponent...>(ToEnttEntity(this->_entity));
    }

    /**
     * Get components of type TComponent from the entity.
     *
     * @tparam  TComponent  components to get
     * @return  components of type TComponent from the entity
     */
    template <typename... TComponent> inline decltype(auto) GetComponents(Registry &registry)
    {
        return registry.GetRegistry().get<TComponent...>(ToEnttEntity(this->_entity));
    }

    inline static entity_id_type FromEnttEntity(entt::entity e) { return static_cast<entity_id_type>(e); }

    inline static entt::entity ToEnttEntity(entity_id_type e) { return static_cast<entt::entity>(e); }

  private:
    entity_id_type _entity;
};
} // namespace ES::Engine
