#include "CollisionStorage.hpp"

namespace ES::Plugin::Collision::Resource {
void CollisionStorage::AddCollisionPair(ES::Engine::Entity::entity_id_type entityA,
                                        ES::Engine::Entity::entity_id_type entityB)
{
    this->_entityCollisionsPairs[entityA].insert(entityB);
    this->_entityCollisionsPairs[entityB].insert(entityA);
}

void CollisionStorage::RemoveCollisionPair(ES::Engine::Entity::entity_id_type entityA,
                                           ES::Engine::Entity::entity_id_type entityB)
{
    this->_entityCollisionsPairs[entityA].erase(entityB);
    this->_entityCollisionsPairs[entityB].erase(entityA);
}

void CollisionStorage::ClearAllCollisionPair()
{
    for (auto &pair : this->_entityCollisionsPairs)
    {
        pair.second.clear();
    }
}

void CollisionStorage::ClearAll() { this->_entityCollisionsPairs.clear(); }

bool CollisionStorage::IsCollidingPair(ES::Engine::Entity::entity_id_type entityA,
                                       ES::Engine::Entity::entity_id_type entityB) const
{
    return this->_entityCollisionsPairs.contains(entityA) && this->_entityCollisionsPairs.at(entityA).contains(entityB);
}
} // namespace ES::Plugin::Collision::Resource
