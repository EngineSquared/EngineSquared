#pragma once

#include "core/Core.hpp"

namespace CameraMovement::System {

/**
 * @brief System that resets the camera behavior to default when requested.
 *
 * This system listens for a specific input (e.g., a key press) to reset
 * the camera behavior to its default state.
 * @param core The engine core.
 */
void ResetCameraBehavior(Engine::Core &core);

} // namespace CameraMovement::System
