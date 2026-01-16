#include "VehicleInput.hpp"

#include "resource/InputManager.hpp"
#include "component/PlayerVehicle.hpp"
#include "component/VehicleController.hpp"

void VehicleInput(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    auto &inputManager = core.GetResource<Input::Resource::InputManager>();

    auto view = registry.view<PlayerVehicle, Physics::Component::VehicleController>();

    for (auto entity : view) {
        auto &controller = view.get<Physics::Component::VehicleController>(entity);

        controller.ResetInputs();

        if (inputManager.IsKeyPressed(GLFW_KEY_W) || inputManager.IsKeyPressed(GLFW_KEY_Z)) {
            controller.SetForward(1.0f);
        }

        if (inputManager.IsKeyPressed(GLFW_KEY_S)) {
            controller.SetForward(-1.0f);
        }

        if (inputManager.IsKeyPressed(GLFW_KEY_A) || inputManager.IsKeyPressed(GLFW_KEY_Q)) {
            controller.SetSteering(1.0f);
        }

        if (inputManager.IsKeyPressed(GLFW_KEY_D)) {
            controller.SetSteering(-1.0f);
        }

        if (inputManager.IsKeyPressed(GLFW_KEY_SPACE)) {
            controller.SetHandBrake(1.0f);
        }
    }
}