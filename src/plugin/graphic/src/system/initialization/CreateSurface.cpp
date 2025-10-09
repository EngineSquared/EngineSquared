#include "CreateSurface.hpp"
#include "resource/Window.hpp"
#include "resource/Context.hpp"
#include "resource/Surface.hpp"
#include "resource/GraphicSettings.hpp"
#include <glfw3webgpu.h>

void Plugin::Graphic::System::CreateSurface(Engine::Core &core) {
    auto &context = core.GetResource<Resource::Context>();
    auto &graphicSettings = core.GetResource<Resource::GraphicSettings>();
    auto &instance = context.instance.value();

    wgpu::Surface surface = nullptr;

    if (graphicSettings.GetWindowSystem() == Resource::WindowSystem::None) {
        return;
    }
    else if (graphicSettings.GetWindowSystem() == Resource::WindowSystem::GLFW) {
        auto glfwWindow = core.GetResource<Window::Resource::Window>().GetGLFWWindow();
        surface = glfwCreateWindowWGPUSurface(instance, glfwWindow);
    }

	if (surface == nullptr) throw std::runtime_error("Could not create surface");

    context.surface = Resource::Surface(surface);
}
