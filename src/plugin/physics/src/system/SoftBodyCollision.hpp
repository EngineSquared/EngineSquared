#pragma once

#include <glm/glm.hpp>

#include "Registry.hpp"

namespace ES::Plugin::Collision::System {
/**
 * @brief Applies collision detection between soft bodies particles and box colliders.
 *
 * @param registry  registry used to get all soft body nodes
 */
void SoftBodyCollision(ES::Engine::Registry &registry);
} // namespace ES::Plugin::Collision::System
