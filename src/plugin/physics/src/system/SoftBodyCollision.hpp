#pragma once

#include <glm/glm.hpp>

#include "Registry.hpp"

namespace ES::Plugin::Physics::System {
/**
 * @brief Detects collision between soft bodies particles and box colliders.
 *
 * @param registry  registry used to get all soft body nodes
 */
void DetectSoftBodyCollisions(ES::Engine::Registry &registry);

/**
 * @brief Applies collisions between soft bodies particles and box colliders.
 *
 * @param registry  registry used to get all soft body nodes
 */
void ApplySoftBodyCollisions(ES::Engine::Registry &registry);

/**
 * @brief Deletes all collision entities.
 *
 * @param registry  registry used to get all collision entities
 */
void DeleteSoftBodyCollisions(ES::Engine::Registry &registry);
} // namespace ES::Plugin::Physics::System
