// Common helpers for Rmlui demo entry points.
#pragma once

#include "GLFW/glfw3.h"
#include "Logger.hpp"
#include "component/Camera.hpp"
#include "component/Transform.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "glm/ext/vector_float3.hpp"
#include "resource/CameraManager.hpp"
#include "resource/InputManager.hpp"
#include "resource/UIContext.hpp"
#include "resource/Window.hpp"

#include <cstdint>
#include <string>

namespace RmluiUsage::Demo {
inline constexpr uint32_t kDefaultWindowWidth = 1280;
inline constexpr uint32_t kDefaultWindowHeight = 720;

inline void EscapeKeySystem(Engine::Core &core)
{
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();
    if (inputManager.IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        core.Stop();
    }
}

inline void ConfigureWindowAndCamera(Engine::Core &core, uint32_t width, uint32_t height)
{
    auto &window = core.GetResource<Window::Resource::Window>();
    window.SetSize(static_cast<int>(width), static_cast<int>(height));

    auto camera = core.CreateEntity();
    constexpr float kDefaultCameraZ = -2.0F;
    camera.AddComponent<Object::Component::Transform>(glm::vec3(0.0F, 0.0F, kDefaultCameraZ));
    camera.AddComponent<Object::Component::Camera>();

    auto &cameraManager = core.GetResource<CameraMovement::Resource::CameraManager>();
    cameraManager.SetActiveCamera(camera);
}

inline void ConfigureDefaultWindowAndCamera(Engine::Core &core)
{
    ConfigureWindowAndCamera(core, kDefaultWindowWidth, kDefaultWindowHeight);
}

inline void LoadDefaultFonts(Rmlui::Resource::UIContext &rmluiContext)
{
    rmluiContext.SetFont("asset/LatoLatin-Regular.ttf");
    rmluiContext.SetFont("asset/LatoLatin-Bold.ttf");
    rmluiContext.SetFont("asset/LatoLatin-Italic.ttf");
    rmluiContext.SetFont("asset/LatoLatin-BoldItalic.ttf");
    rmluiContext.SetFont("asset/NotoEmoji-Regular.ttf");
}

inline void RegisterHoverClick(Rmlui::Resource::UIContext &rmluiContext, const char *demoName)
{
    if (auto *hoverLogo = rmluiContext.GetElementById("hover-logo"))
    {
        std::string message = std::string(demoName) + " demo hover clicked";
        rmluiContext.RegisterEventListener(*hoverLogo, "click", [message](auto &) { Log::Info(message); });
    }
}

inline void AttachHoverOverlay(Rmlui::Resource::UIContext &rmluiContext, const char *demoName)
{
    rmluiContext.LoadOverlayDocument("asset/hover_esq.rml");
    RegisterHoverClick(rmluiContext, demoName);
}
} // namespace RmluiUsage::Demo
