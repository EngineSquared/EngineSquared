#pragma once

#include "entity/Entity.hpp"

namespace Physics::Event {

/**
 * @brief Event triggered when a collision starts between two entities.
 */
struct CollisionAddedEvent {
    Engine::Entity entity1; ///< The first entity involved in the collision.
    Engine::Entity entity2; ///< The second entity involved in the collision.
};

/**
 * @brief Event triggered when a collision persists between two entities.
 */
struct CollisionPersistedEvent {
    Engine::Entity entity1; ///< The first entity involved in the collision.
    Engine::Entity entity2; ///< The second entity involved in the collision.
};

/**
 * @brief Event triggered when a collision ends between two entities.
 */
struct CollisionRemovedEvent {
    Engine::Entity entity1; ///< The first entity involved in the collision.
    Engine::Entity entity2; ///< The second entity involved in the collision.
};

} // namespace Physics::Event
