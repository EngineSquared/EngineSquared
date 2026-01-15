#pragma once

#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "resource/CameraManager.hpp"
#include "utils/CameraBehavior.hpp"

namespace CameraMovement::Utils {

/**
 * @brief Camera behavior that does not move the camera.
 */
class DontMoveBehavior : public ICameraBehavior {
  public:
    DontMoveBehavior() = default;
    ~DontMoveBehavior() override = default;

    void Update(Engine::Core &core, Resource::CameraManager &manager, Object::Component::Transform &transform,
                Object::Component::Camera &camera, float deltaTime) override
    {
        // Intentionally does nothing to keep the camera static
    }
};

} // namespace CameraMovement::Utils
