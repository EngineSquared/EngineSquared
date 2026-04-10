#include "entity/Entity.hpp"

Engine::Entity::Entity(Core &core, EntityId entityId) : _core(core), _entityId(entityId) {}

bool Engine::Entity::IsAlive() const { return GetCore().IsEntityValid(_entityId); }

void Engine::Entity::Kill() { GetCore().KillEntity(_entityId); }

Engine::EntityId Engine::Entity::Id() const { return _entityId; }

Engine::Entity::operator EntityId() const { return _entityId; }

void Engine::Entity::RemoveTemporaryComponents(Core &core) { EntityId::RemoveTemporaryComponents(core); }

bool Engine::Entity::operator==(const Entity &rhs) const { return _entityId.value == rhs._entityId.value; }

bool Engine::Entity::operator==(const EntityId &rhs) const { return _entityId.value == rhs.value; }

Engine::Core &Engine::Entity::GetCore() const { return _core.get(); }
