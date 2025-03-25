#pragma once

#include <glm/glm.hpp>

#include "Core.hpp"

namespace ES::Plugin::Physics::System {
/**
 * @brief Apply gravity to all soft body nodes in the registry.
 *
 * @param core  core used to get all soft body nodes
 */
void VelocityIntegration(ES::Engine::Core &core);
} // namespace ES::Plugin::Physics::System
