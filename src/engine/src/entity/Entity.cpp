#include "Entity.hpp"

bool ES::Engine::Entity::IsValid() const { return this->_entity != ES::Engine::Entity::entity_null_id; }
