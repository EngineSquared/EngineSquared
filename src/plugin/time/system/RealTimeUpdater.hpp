#pragma once

#include "RealTimeProvider.hpp"
#include "Registry.hpp"

namespace ES::Plugin::Time::System {
/**
 * @brief Update the real time providers in the registry
 *
 * @param registry  registry used to get all real time providers
 */
void RealTimeUpdater(ES::Engine::Registry &registry);
} // namespace ES::Plugin::Time::System
