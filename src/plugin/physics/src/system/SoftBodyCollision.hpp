#pragma once

#include <glm/glm.hpp>

#include "Core.hpp"

namespace ES::Plugin::Physics::System {
/**
 * @brief Detects collision between soft bodies particles and box colliders.
 *
 * @param core  core used to get all soft body nodes
 */
void DetectSoftBodyCollisions(ES::Engine::Core &core);

/**
 * @brief Applies collisions between soft bodies particles and box colliders.
 *
 * @param core  core used to get all soft body nodes
 */
void ApplySoftBodyCollisions(ES::Engine::Core &core);

/**
 * @brief Deletes all collision entities.
 *
 * @param core  core used to get all collision entities
 */
void DeleteSoftBodyCollisions(ES::Engine::Core &core);
} // namespace ES::Plugin::Physics::System
