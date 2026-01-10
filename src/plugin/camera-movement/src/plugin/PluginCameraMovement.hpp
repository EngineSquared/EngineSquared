#pragma once

#include "plugin/APlugin.hpp"

namespace CameraMovement {

/**
 * @brief Plugin for camera movement control.
 *
 * This plugin provides camera movement functionality with customizable behaviors.
 * It requires Object, Input, and Window plugins to be loaded.
 *
 * After binding the plugin, the user must set the active camera entity using:
 * @code
 * auto& cameraManager = core.GetResource<CameraMovement::Resource::CameraManager>();
 * cameraManager.SetActiveCamera(cameraEntity);
 * @endcode
 */
class Plugin : public Engine::APlugin {
  public:
    explicit Plugin(Engine::Core &core);
    ~Plugin() override = default;

    void Bind() override;
};

} // namespace CameraMovement
