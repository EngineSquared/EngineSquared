#pragma once

#include "Engine.hpp"

namespace Plugin::Scene::System {
/**
 * @brief Unloads current scene and loads the new one.
 *
 * @param   core The core to load and unload.
 */
void UpdateScene(Engine::Core &core);
} // namespace Plugin::Scene::System
