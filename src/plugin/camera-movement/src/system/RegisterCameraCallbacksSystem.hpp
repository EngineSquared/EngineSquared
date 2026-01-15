#pragma once

#include "core/Core.hpp"

namespace CameraMovement::System {

/**
 * @brief Startup system that registers mouse callbacks for camera control.
 *
 * This system registers mouse button and cursor position callbacks to enable
 * camera rotation via right-mouse-button dragging.
 *
 * @param core The engine core.
 */
void RegisterCameraCallbacksSystem(Engine::Core &core);

} // namespace CameraMovement::System
