#pragma once

#include "Registry.hpp"

namespace ES::Plugin::Collision::System {
/**
 * Collision system used to check for collisions between entities with a BoxCollider3D component
 *
 * @param	registry	registry used to get entities to check
 */
void CollisionCheckerABAB(ES::Engine::Registry &registry);

/**
 * Remove all ABAB collisions from the registry
 *
 * @param	registry	registry used to get entities to check
 */
void RemoveABABCollisions(ES::Engine::Registry &registry);
} // namespace ES::Plugin::Collision::System
