#pragma once

#include <glm/glm.hpp>

#include "Core.hpp"

namespace ES::Plugin::Physics::System {
/**
 * @brief Detects collision between soft bodies particles and box colliders.
 *
 * @param registry  registry used to get all soft body nodes
 */
void DetectSoftBodyCollisions(ES::Engine::Core &registry);

/**
 * @brief Applies collisions between soft bodies particles and box colliders.
 *
 * @param registry  registry used to get all soft body nodes
 */
void ApplySoftBodyCollisions(ES::Engine::Core &registry);

/**
 * @brief Deletes all collision entities.
 *
 * @param registry  registry used to get all collision entities
 */
void DeleteSoftBodyCollisions(ES::Engine::Core &registry);
} // namespace ES::Plugin::Physics::System
