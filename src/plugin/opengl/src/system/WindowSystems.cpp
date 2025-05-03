#include "WindowSystems.hpp"
#include "Window.hpp"
#include "Camera.hpp"
#include "MouseDragging.hpp"
#include "Input.hpp"
#include "TextureManager.hpp"
#include "MaterialCache.hpp"
#include "ShaderManager.hpp"
#include "Light.hpp"
#include "TextureHandle.hpp"

#include <glm/gtc/type_ptr.hpp>

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

        dragging.lastMousePos.x = xpos;
        dragging.lastMousePos.y = ypos;
    });
}

void ES::Plugin::OpenGL::System::CreateCamera(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::Camera>(Resource::Camera(DEFAULT_WIDTH, DEFAULT_HEIGHT));
}

void ES::Plugin::OpenGL::System::LoadMaterialCache(ES::Engine::Core &core)
{
    auto &materialCache = core.RegisterResource<Resource::MaterialCache>({});
    materialCache.Add(entt::hashed_string("default"), std::move(Utils::Material()));
}

void ES::Plugin::OpenGL::System::UpdateMatrices(ES::Engine::Core &core)
{
    auto &cam = core.GetResource<Resource::Camera>();
    cam.view = glm::lookAt(cam.viewer.getViewPoint(), cam.viewer.getViewCenter(), cam.viewer.getUpVector());
    cam.projection = glm::perspective(glm::radians(45.0f), cam.size.x / cam.size.y, 0.1f, 100.0f);
}

void ES::Plugin::OpenGL::System::SetupLights(ES::Engine::Core &core)
{
    auto &shader = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"default"});

    std::array<ES::Plugin::OpenGL::Utils::Light, 5> light = {
        ES::Plugin::OpenGL::Utils::Light{glm::vec4(0, 0, 0, 1), glm::vec3(0.0f, 0.8f, 0.8f)},
        ES::Plugin::OpenGL::Utils::Light{glm::vec4(0, 0, 0, 1), glm::vec3(0.0f, 0.0f, 0.8f)},
        ES::Plugin::OpenGL::Utils::Light{glm::vec4(0, 0, 0, 1), glm::vec3(0.8f, 0.0f, 0.0f)},
        ES::Plugin::OpenGL::Utils::Light{glm::vec4(0, 0, 0, 1), glm::vec3(0.0f, 0.8f, 0.0f)},
        ES::Plugin::OpenGL::Utils::Light{glm::vec4(0, 0, 0, 1), glm::vec3(0.8f, 0.8f, 0.8f)}
    };

    float nbr_lights = 5.f;
    float scale = 2.f * glm::pi<float>() / nbr_lights;

    light[0].position = glm::vec4(5.f * cosf(scale * 0.f), 5.f, 5.f * sinf(scale * 0.f), 1.f);
    light[1].position = glm::vec4(5.f * cosf(scale * 1.f), 5.f, 5.f * sinf(scale * 1.f), 1.f);
    light[2].position = glm::vec4(5.f * cosf(scale * 2.f), 5.f, 5.f * sinf(scale * 2.f), 1.f);
    light[3].position = glm::vec4(5.f * cosf(scale * 3.f), 5.f, 5.f * sinf(scale * 3.f), 1.f);
    light[4].position = glm::vec4(5.f * cosf(scale * 4.f), 5.f, 5.f * sinf(scale * 4.f), 1.f);

    shader.use();
    for (int i = 0; i < 5; i++)
    {
        glUniform4fv(shader.uniform(fmt::format("Light[{}].Position", i).c_str()), 1,
                     glm::value_ptr(light[i].position));
        glUniform3fv(shader.uniform(fmt::format("Light[{}].Intensity", i).c_str()), 1,
                     glm::value_ptr(light[i].intensity));
    }
    shader.disable();
}

void ES::Plugin::OpenGL::System::SetupCamera(ES::Engine::Core &core)
{
    auto &shaderProgram = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"default"});
    shaderProgram.use();
    glUniform3fv(shaderProgram.uniform("CamPos"), 1,
                 glm::value_ptr(core.GetResource<Resource::Camera>().viewer.getViewPoint()));
    shaderProgram.disable();
}

void ES::Plugin::OpenGL::System::GLClearColor(const ES::Engine::Core &) { glClear(GL_COLOR_BUFFER_BIT); }

void ES::Plugin::OpenGL::System::GLClearDepth(const ES::Engine::Core &) { glClear(GL_DEPTH_BUFFER_BIT); }

void ES::Plugin::OpenGL::System::GLEnableDepth(const ES::Engine::Core &) { glEnable(GL_DEPTH_TEST); }

void ES::Plugin::OpenGL::System::GLEnableCullFace(const ES::Engine::Core &)
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}