#include "CreateSurface.hpp"
#include "exception/SurfaceCreationError.hpp"
#include "resource/Context.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/Instance.hpp"
#include "resource/Surface.hpp"
#include "resource/Window.hpp"
#include <glfw3webgpu.h>

void Graphic::System::CreateSurface(Engine::Core &core)
{
    auto &context = core.GetResource<Resource::Context>();
    auto &graphicSettings = core.GetResource<Resource::GraphicSettings>();

    if (!core.HasResource<Resource::Instance>())
    {
        Log::Error("Graphic::System::CreateSurface: context.instance has no value");
        return;
    }
    auto &instance = core.GetResource<Resource::Instance>();
    wgpu::Surface surface = nullptr;

    if (graphicSettings.GetWindowSystem() == Resource::WindowSystem::None)
        return;
    else if (graphicSettings.GetWindowSystem() == Resource::WindowSystem::GLFW)
    {
        auto glfwWindow = core.GetResource<Window::Resource::Window>().GetGLFWWindow();
        surface = glfwCreateWindowWGPUSurface(*instance, glfwWindow);
    }
    else
    {
        throw Exception::SurfaceCreationError(fmt::format("Unsupported window system for surface creation: {}",
                                                          static_cast<int>(graphicSettings.GetWindowSystem())));
    }

    if (surface == nullptr)
        throw Exception::SurfaceCreationError("Could not create surface");

    core.RegisterResource(Resource::Surface(surface));
}
