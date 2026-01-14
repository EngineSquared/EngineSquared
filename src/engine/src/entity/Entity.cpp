#include "entity/Entity.hpp"

bool Engine::Entity::IsValid() const { return _core.IsEntityValid(_entity); }
