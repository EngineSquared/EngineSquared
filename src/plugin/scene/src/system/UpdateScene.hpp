#pragma once

#include "Engine.hpp"

namespace ES::Plugin::Scene::System {
/**
 * @brief Unloads current scene and loads the new one.
 *
 * @param   core The registry to load and unload.
 */
void UpdateScene(ES::Engine::Core &core);
} // namespace ES::Plugin::Scene::System
