#pragma once

#include <set>
#include <unordered_map>

#include "Entity.hpp"
#include "Registry.hpp"

namespace ES::Plugin::Collision::Resource {
/**
 * Resource class used to store collisions between entities
 */
class CollisionStorage {
  public:
    CollisionStorage() = default;
    ~CollisionStorage() = default;

    /**
     * Add collision information to the storage
     *
     * @param	entityA	collided entity A
     * @param	entityB collided entity B
     */
    void AddCollisionPair(ES::Engine::Entity::entity_id_type entityA, ES::Engine::Entity::entity_id_type entityB);

    /**
     * Remove collision information to the storage
     *
     * @param	entityA	collided entity A
     * @param	entityB collided entity B
     */
    void RemoveCollisionPair(ES::Engine::Entity::entity_id_type entityA, ES::Engine::Entity::entity_id_type entityB);

    /**
     * Remove all collision pairs, meaning the entity entry still exists but is no longer linked to another entity
     */
    void ClearAllCollisionPair();

    /**
     * Clear all entity which can collide with others
     */
    void ClearAll();

    /**
     * Check whenever an entity collide with another entity
     *
     * @param	entityA	the entity A to check
     * @param	entityB	the entity B to check
     * @return	true if they collide with each other, false otherwise
     */
    bool IsCollidingPair(ES::Engine::Entity::entity_id_type entityA, ES::Engine::Entity::entity_id_type entityB) const;

  private:
    std::unordered_map<ES::Engine::Entity::entity_id_type, std::set<ES::Engine::Entity::entity_id_type>>
        _entityCollisionsPairs;
};
} // namespace ES::Plugin::Collision::Resource
