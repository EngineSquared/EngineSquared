#pragma once

#include "Engine.hpp"

namespace Graphic::System {

/**
 * @brief Updates GPU mesh buffers for dynamic meshes (soft bodies, etc.)
 *
 * This system updates the vertex data in GPU buffers when the mesh vertices
 * change during simulation (e.g., soft body physics).
 */
void UpdateGPUMeshes(Engine::Core &core);

} // namespace Graphic::System
