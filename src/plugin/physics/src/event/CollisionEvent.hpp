#pragma once

#include "entity/Entity.hpp"

namespace Physics::Event {

struct CollisionAddedEvent {
    Engine::Entity entity1;
    Engine::Entity entity2;
};

struct CollisionPersistedEvent {
    Engine::Entity entity1;
    Engine::Entity entity2;
};

struct CollisionRemovedEvent {
    Engine::Entity entity1;
    Engine::Entity entity2;
};

} // namespace Physics::Event
