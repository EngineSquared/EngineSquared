#include "entity/Entity.hpp"

bool Engine::Entity::IsAlive() const { return GetCore().IsEntityValid(_entityId); }

void Engine::Entity::Kill() { GetCore().KillEntity(_entityId); }
