#include "entity/Entity.hpp"

namespace Engine {

Entity::Entity(Core &core, EntityId entityId) : _core(core), _entityId(entityId) {}

bool Entity::IsAlive() const { return GetCore().IsEntityValid(_entityId); }

void Entity::Kill() { GetCore().KillEntity(_entityId); }

EntityId Entity::Id() const { return _entityId; }

Entity::operator EntityId() const { return _entityId; }

void Entity::RemoveTemporaryComponents(Core &core) { EntityId::RemoveTemporaryComponents(core); }

bool Entity::operator==(const Entity &rhs) const { return _entityId.value == rhs._entityId.value; }

bool Entity::operator==(const EntityId &rhs) const { return _entityId.value == rhs.value; }

Core &Entity::GetCore() const { return _core.get(); }

} // namespace Engine
