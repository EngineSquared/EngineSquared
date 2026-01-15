#pragma once

#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"

namespace CameraMovement::Resource {
class CameraManager;
}

namespace CameraMovement::Utils {

/**
 * @brief Base interface for camera behaviors.
 *
 * Derived classes should implement the Update method to control camera movement.
 */
class ICameraBehavior {
  public:
    virtual ~ICameraBehavior() = default;

    /**
     * @brief Update the camera behavior.
     *
     * @param core The engine core.
     * @param manager The camera manager.
     * @param transform The transform component of the camera.
     * @param camera The camera component.
     * @param deltaTime The time elapsed since the last frame.
     */
    virtual void Update(Engine::Core &core, Resource::CameraManager &manager, Object::Component::Transform &transform,
                        Object::Component::Camera &camera, float deltaTime) = 0;
};

} // namespace CameraMovement::Utils
