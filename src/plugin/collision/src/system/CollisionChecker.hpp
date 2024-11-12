#pragma once

#include "Registry.hpp"

namespace ES::Plugin::Collision::System {
/**
 * Collision system used to check for collisions between entities with a BoxCollider3D component
 *
 * @param	registry	registry used to get entities to check
 */
void CollisionChecker(ES::Engine::Registry &registry);

/**
 * Reset all collisions
 * Should be used just before CollisionChecker system
 */
void ResetCollision(ES::Engine::Registry &registry);
} // namespace ES::Plugin::Collision::System
