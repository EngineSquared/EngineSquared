#pragma once

#include "component/Camera.hpp"
#include "component/CameraBehavior.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "resource/CameraManager.hpp"

namespace CameraMovement::Component {

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
    }
};

} // namespace CameraMovement::Component
