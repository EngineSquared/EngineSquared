#pragma once

#include "core/Core.hpp"

namespace CameraMovement::System {

/**
 * @brief System that controls the camera based on the active behavior.
 *
 * This system runs in the Update scheduler and applies camera transformations
 * based on user input and the selected camera behavior.
 *
 * @param core The engine core.
 */
void CameraControlSystem(Engine::Core &core);

} // namespace CameraMovement::System
