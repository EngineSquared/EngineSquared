#include "VehicleInput.hpp"

#include "component/PlayerVehicle.hpp"
#include "component/VehicleController.hpp"
#include "resource/InputManager.hpp"

void VehicleInput(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    auto &inputManager = core.GetResource<Input::Resource::InputManager>();

    auto view = registry.view<PlayerVehicle, Physics::Component::VehicleController>();

    for (auto entity : view)
    {
        auto &controller = view.get<Physics::Component::VehicleController>(entity);

        controller.ResetInputs();

        float forward = 0.0f;
        float steering = 0.0f;
        float handbrake = 0.0f;

        if (inputManager.IsKeyPressed(GLFW_KEY_W) || inputManager.IsKeyPressed(GLFW_KEY_Z))
        {
            forward += 1.0f;
        }

        if (inputManager.IsKeyPressed(GLFW_KEY_S))
        {
            forward -= 1.0f;
        }

        if (inputManager.IsKeyPressed(GLFW_KEY_A) || inputManager.IsKeyPressed(GLFW_KEY_Q))
        {
            steering += 1.0f;
        }

        if (inputManager.IsKeyPressed(GLFW_KEY_D))
        {
            steering -= 1.0f;
        }

        if (inputManager.IsKeyPressed(GLFW_KEY_SPACE))
        {
            handbrake = 1.0f;
        }

        controller.SetForward(forward);
        controller.SetSteering(steering);
        controller.SetHandBrake(handbrake);
    }
}
