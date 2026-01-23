#pragma once

#include <cmath>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "component/Camera.hpp"
#include "component/PlayerVehicle.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "resource/CameraManager.hpp"
#include "resource/InputManager.hpp"
#include "utils/CameraBehavior.hpp"
#include "utils/CameraUtils.hpp"

/**
 * @brief Chase camera behavior for vehicles.
 */
class ChaseCameraBehavior : public CameraMovement::Utils::ICameraBehavior {
  public:
    explicit ChaseCameraBehavior(Engine::Entity vehicleEntity) : _vehicleEntity(vehicleEntity) {}
    ~ChaseCameraBehavior() override = default;

    void Update(Engine::Core &core, CameraMovement::Resource::CameraManager &manager,
                Object::Component::Transform &transform, Object::Component::Camera &camera, float deltaTime) override
    {
        auto &registry = core.GetRegistry();

        auto &vehicleTransform = registry.get<Object::Component::Transform>(GetVehicleEntity());

        glm::vec3 vehiclePos = vehicleTransform.GetPosition();
        glm::quat vehicleRot = vehicleTransform.GetRotation();

        glm::vec3 vehicleForward = vehicleRot * glm::vec3(0.0f, 0.0f, 1.0f);

        constexpr float cameraDistance = 8.0f;
        constexpr float cameraHeight = 3.0f;
        constexpr float cameraLookAhead = 2.0f;

        glm::vec3 cameraOffset = -vehicleForward * cameraDistance + glm::vec3(0.0f, cameraHeight, 0.0f);
        glm::vec3 cameraPosition = vehiclePos + cameraOffset;
        glm::vec3 cameraTarget = vehiclePos + vehicleForward * cameraLookAhead + glm::vec3(0.0f, 0.5f, 0.0f);

        transform.SetPosition(cameraPosition);

        glm::quat lookRotation =
            CameraMovement::Utils::ComputeLookAtQuaternion(cameraPosition, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
        transform.SetRotation(lookRotation);
    }

    Engine::Entity GetVehicleEntity() const { return _vehicleEntity; }

    void SetVehicleEntity(Engine::Entity vehicleEntity) { _vehicleEntity = vehicleEntity; }

  private:
    Engine::Entity _vehicleEntity;
};
