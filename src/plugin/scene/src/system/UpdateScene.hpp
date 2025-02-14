#pragma once

#include "Engine.hpp"

namespace ES::Plugin::Scene::System {
/**
 * @brief Unloads current scene and loads the new one.
 *
 * @param   reg The registry to load and unload.
 */
void UpdateScene(ES::Engine::Core &reg);
} // namespace ES::Plugin::Scene::System
