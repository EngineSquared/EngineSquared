#pragma once

#include <glm/glm.hpp>

#include "Core.hpp"

namespace ES::Plugin::Physics::System {
/**
 * @brief Apply gravity to all soft body nodes in the registry.
 *
 * @param registry  registry used to get all soft body nodes
 */
void VelocityIntegration(ES::Engine::Core &registry);
} // namespace ES::Plugin::Physics::System
