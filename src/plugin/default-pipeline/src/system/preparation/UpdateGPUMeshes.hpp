#pragma once

#include "Engine.hpp"

namespace DefaultPipeline::System {

/**
 * @brief Updates GPU mesh buffers for dynamic meshes (soft bodies, etc.)
 *
 * This system updates the vertex data in GPU buffers when the mesh vertices
 * change during simulation (e.g., soft body physics).
 */
void UpdateGPUMeshes(Engine::Core &core);

} // namespace DefaultPipeline::System
