#include "entity/Entity.hpp"

bool Engine::Entity::IsValid() const { return this->_entity != Engine::Entity::entity_null_id; }
