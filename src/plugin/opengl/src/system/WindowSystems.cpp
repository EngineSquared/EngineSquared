#include "WindowSystems.hpp"
#include "Camera.hpp"
#include "Input.hpp"
#include "MaterialCache.hpp"
#include "MouseDragging.hpp"
#include "ShaderManager.hpp"
#include "TextureHandle.hpp"
#include "TextureManager.hpp"
#include "Window.hpp"

void ES::Plugin::OpenGL::System::InitGLEW(const ES::Engine::Core &)
{
    if (GLenum err = glewInit(); GLEW_OK != err)
    {
        ES::Utils::Log::Error(fmt::format("Error: {}", reinterpret_cast<const char *>(glewGetErrorString(err))));
    }
    ES::Utils::Log::Info(
        fmt::format("Status: Using GLEW {}", reinterpret_cast<const char *>(glewGetString(GLEW_VERSION))));
}

void ES::Plugin::OpenGL::System::CheckGLEWVersion(const ES::Engine::Core &)
{
    if (!glewIsSupported("GL_VERSION_4_2"))
    {
        ES::Utils::Log::Error("OpenGL 4.2 not supported");
        return;
    }
    ES::Utils::Log::Debug("OpenGL 4.2 supported");
}

void ES::Plugin::OpenGL::System::SetupResizeViewport(ES::Engine::Core &core)
{
    core.GetResource<Window::Resource::Window>().SetFramebufferSizeCallback(
        &core, [](GLFWwindow *window, int width, int height) {
            auto &c = *static_cast<ES::Engine::Core *>(glfwGetWindowUserPointer(window));
            c.GetResource<OpenGL::Resource::Camera>().viewer.setAspectRatio(static_cast<float>(width) /
                                                                            static_cast<float>(height));
            c.GetResource<Resource::Camera>().size = glm::vec2(width, height);
            glViewport(0, 0, width, height);
        });
}

void ES::Plugin::OpenGL::System::SetupMouseDragging(ES::Engine::Core &core)
{
    core.RegisterResource<ES::Plugin::OpenGL::Utils::MouseDragging>(ES::Plugin::OpenGL::Utils::MouseDragging());

    auto &inputManager = core.GetResource<ES::Plugin::Input::Resource::InputManager>();

    auto handleLeftDrag = [](auto &camera, const auto &dragging, double xpos, double ypos) {
        float dx = static_cast<float>(xpos - dragging.lastMousePos.x) / camera.size.x;
        float dy = static_cast<float>(dragging.lastMousePos.y - ypos) / camera.size.y;
        camera.viewer.rotate(dx, dy);
    };

    auto handleMiddleDrag = [](auto &camera, const auto &dragging, double /*xpos*/, double ypos) {
        float dy = static_cast<float>(dragging.lastMousePos.y - ypos) / camera.size.y;
        camera.viewer.zoom(dy);
    };

    auto handleRightDrag = [](auto &camera, const auto &dragging, double xpos, double ypos) {
        float dx = static_cast<float>(xpos - dragging.lastMousePos.x) / camera.size.x;
        float dy = static_cast<float>(dragging.lastMousePos.y - ypos) / camera.size.y;
        camera.viewer.translate(-dx, -dy, true);
    };

    inputManager.RegisterCursorPosCallback([=](ES::Engine::Core &cbCore, double xpos, double ypos) {
        auto &dragging = cbCore.GetResource<ES::Plugin::OpenGL::Utils::MouseDragging>();
        auto &camera = cbCore.GetResource<ES::Plugin::OpenGL::Resource::Camera>();

        if (ES::Plugin::Input::Utils::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
            handleLeftDrag(camera, dragging, xpos, ypos);
        else if (ES::Plugin::Input::Utils::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE))
            handleMiddleDrag(camera, dragging, xpos, ypos);
        else if (ES::Plugin::Input::Utils::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
            handleRightDrag(camera, dragging, xpos, ypos);

        dragging.lastMousePos.x = static_cast<float>(xpos);
        dragging.lastMousePos.y = static_cast<float>(ypos);
    });
}

void ES::Plugin::OpenGL::System::GLClearColor(const ES::Engine::Core &) { glClear(GL_COLOR_BUFFER_BIT); }

void ES::Plugin::OpenGL::System::GLClearDepth(const ES::Engine::Core &) { glClear(GL_DEPTH_BUFFER_BIT); }

void ES::Plugin::OpenGL::System::GLEnableDepth(const ES::Engine::Core &) { glEnable(GL_DEPTH_TEST); }

void ES::Plugin::OpenGL::System::GLEnableCullFace(const ES::Engine::Core &)
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}
