#pragma once

#include "Core.hpp"

namespace ES::Plugin::Physics::System {
/**
 * Collision system used to check for collisions between entities with a BoxCollider3D component
 *
 * @see ES::Plugin::Physics::Component::BoxCollider3D
 *
 * @param	core	core used to get entities to check
 */
void DetectABABCollisions(ES::Engine::Core &core);

/**
 * Remove all ABAB collisions from the core
 *
 * @param	core	core used to get entities to check
 */
void RemoveABABCollisions(ES::Engine::Core &core);
} // namespace ES::Plugin::Physics::System
