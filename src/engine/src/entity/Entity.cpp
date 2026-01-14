#include "entity/Entity.hpp"

bool Engine::Entity::IsValid() const {
    if (!_core.has_value()) {
        return false;
    }
    return GetCore().IsEntityValid(_entity);
}
