#include "system/RegisterCameraCallbacksSystem.hpp"

#include <GLFW/glfw3.h>

#include "Logger.hpp"
#include "component/Transform.hpp"
#include "resource/CameraManager.hpp"
#include "resource/InputManager.hpp"
#include "resource/Window.hpp"
#include "utils/CameraUtils.hpp"

namespace CameraMovement::System {

namespace {

void MouseButtonCallback(Engine::Core &core, int button, int action, int /* mods */)
{
    if (button != GLFW_MOUSE_BUTTON_RIGHT)
    {
        return;
    }

    if (!core.GetRegistry().ctx().contains<Resource::CameraManager>())
    {
        Log::Warn("CameraManager resource not found in MouseButtonCallback");
        return;
    }

    auto &cameraManager = core.GetResource<Resource::CameraManager>();

    if (action == GLFW_PRESS)
    {
        cameraManager.SetMouseDragging(true);
        if (cameraManager.HasValidCamera())
        {
            auto entity = cameraManager.GetActiveCamera();
            auto &transform = core.GetRegistry().get<Object::Component::Transform>(entity);
            cameraManager.SetOriginRotation(transform.GetRotation());
        }
    }
    else if (action == GLFW_RELEASE)
    {
        cameraManager.SetMouseDragging(false);
    }
}

void CursorPosCallback(Engine::Core &core, double xpos, double ypos)
{
    if (!core.GetRegistry().ctx().contains<Resource::CameraManager>())
    {
        Log::Warn("CameraManager resource not found in CursorPosCallback");
        return;
    }

    if (!core.GetRegistry().ctx().contains<Window::Resource::Window>())
    {
        Log::Warn("Window resource not found in CursorPosCallback");
        return;
    }

    auto &cameraManager = core.GetResource<Resource::CameraManager>();
    auto &window = core.GetResource<Window::Resource::Window>();

    bool shouldRotate = (window.IsCursorMasked() || cameraManager.IsMouseDragging()) &&
                        cameraManager.HasValidCamera() &&
                        !(window.IsCursorMasked() && !cameraManager.WasCursorMasked());

    if (shouldRotate)
    {
        auto &transform = core.GetRegistry().get<Object::Component::Transform>(cameraManager.GetActiveCamera());
        auto yaw = static_cast<float>((xpos - cameraManager.GetLastMouseX()) * cameraManager.GetMouseSensitivity());
        auto pitch = static_cast<float>((ypos - cameraManager.GetLastMouseY()) * cameraManager.GetMouseSensitivity());

        glm::quat newRotation = Utils::RotateQuaternion(cameraManager.GetOriginRotation(), pitch, yaw);
        transform.SetRotation(newRotation);
        cameraManager.SetOriginRotation(newRotation);
    }

    cameraManager.SetLastMousePosition(xpos, ypos);
    cameraManager.SetWasCursorMasked(window.IsCursorMasked());
}

} // namespace

void RegisterCameraCallbacksSystem(Engine::Core &core)
{
    if (!core.HasResource<Input::Resource::InputManager>())
    {
        Log::Warn("InputManager resource not found, cannot register camera callbacks");
        return;
    }

    auto &inputManager = core.GetResource<Input::Resource::InputManager>();

    inputManager.RegisterMouseButtonCallback(MouseButtonCallback);
    inputManager.RegisterCursorPosCallback(CursorPosCallback);
}

} // namespace CameraMovement::System
