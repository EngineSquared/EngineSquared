#pragma once

#include "ESGLFWWINDOW.hpp"
#include "Entity.hpp"
#include "MaterialCache.hpp"
#include "Model.hpp"
#include "Object.hpp"
#include "ShaderManager.hpp"
#include "core/Core.hpp"
#include "entt/entt.hpp"
#include "resource/Buttons/Buttons.hpp"
#include "resource/Camera/Camera.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace ES::Plugin::ESGL::System {
const int DEFAULT_WIDTH = 800;
const int DEFAULT_HEIGHT = 800;

void UpdateKey(ES::Engine::Core &core)
{
    GLFWwindow *window = core.GetResource<Resource::ESGLFWWINDOW>().window;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void UpdateButton(ES::Engine::Core &core)
{
    GLFWwindow *window = core.GetResource<Resource::ESGLFWWINDOW>().window;
    auto &mouseButtons = core.GetResource<ESGL::Resource::Buttons>().mouse;
    for (auto &button : mouseButtons)
    {
        bool pressed = glfwGetMouseButton(window, button.first) == GLFW_PRESS;
        button.second.updated = button.second.pressed != pressed;
        button.second.pressed = pressed;
    }
}

void SaveLastMousePos(ES::Engine::Core &core)
{
    auto &buttons = core.GetResource<Resource::Buttons>();
    auto &lastMousePos = buttons.lastMousePos;
    auto &mouseButtons = buttons.mouse;
    auto window = core.GetResource<Resource::ESGLFWWINDOW>().window;
    if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT].updated || mouseButtons[GLFW_MOUSE_BUTTON_MIDDLE].updated ||
        mouseButtons[GLFW_MOUSE_BUTTON_RIGHT].updated)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        lastMousePos.x = xpos;
        lastMousePos.y = ypos;
    }
}

void InitGLFW(ES::Engine::Core &core)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }
}

void SetupGLFWHints(ES::Engine::Core &core)
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void CreateGLFWWindow(ES::Engine::Core &core)
{
    if (!core.RegisterResource<Resource::ESGLFWWINDOW>(
                 {glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "OpenGL Framework", nullptr, nullptr)})
             .window)
    {
        glfwTerminate();
        std::cerr << "Failed to create GLFW window" << std::endl;
        return;
    }
}

void LinkGLFWContextToGL(ES::Engine::Core &core)
{
    glfwMakeContextCurrent(core.GetResource<Resource::ESGLFWWINDOW>().window);
}

void InitGLEW(ES::Engine::Core &core)
{
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
}

void CheckGLEWVersion(ES::Engine::Core &core)
{
    if (!glewIsSupported("GL_VERSION_4_2"))
    {
        std::cerr << "OpenGL 4.2 not supported" << std::endl;
        return;
    }
    std::cout << "OpenGL 4.2 supported" << std::endl;
}

void GLFWEnableVSync(ES::Engine::Core &core) { glfwSwapInterval(1); }

void UpdatePosCursor(ES::Engine::Core &core)
{
    auto &currentMousePos = core.GetResource<Resource::Buttons>().currentMousePos;
    glfwGetCursorPos(core.GetResource<Resource::ESGLFWWINDOW>().window, &currentMousePos.x, &currentMousePos.y);
}

// Function to handle mouse dragging interactions
void MouseDragging(ES::Engine::Core &core)
{
    auto &buttons = core.GetResource<Resource::Buttons>();
    auto &lastMousePos = buttons.lastMousePos;
    auto &currentMousePos = buttons.currentMousePos;
    auto &mouseButtons = buttons.mouse;
    auto &camera = core.GetResource<Resource::Camera>();
    if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT].pressed)
    {
        float fractionChangeX =
            static_cast<float>(currentMousePos.x - lastMousePos.x) / static_cast<float>(camera.size.x);
        float fractionChangeY =
            static_cast<float>(lastMousePos.y - currentMousePos.y) / static_cast<float>(camera.size.y);
        camera.viewer.rotate(fractionChangeX, fractionChangeY);
    }
    else if (mouseButtons[GLFW_MOUSE_BUTTON_MIDDLE].pressed)
    {
        float fractionChangeY =
            static_cast<float>(lastMousePos.y - currentMousePos.y) / static_cast<float>(camera.size.y);
        camera.viewer.zoom(fractionChangeY);
    }
    else if (mouseButtons[GLFW_MOUSE_BUTTON_RIGHT].pressed)
    {
        float fractionChangeX =
            static_cast<float>(currentMousePos.x - lastMousePos.x) / static_cast<float>(camera.size.x);
        float fractionChangeY =
            static_cast<float>(lastMousePos.y - currentMousePos.y) / static_cast<float>(camera.size.y);
        camera.viewer.translate(-fractionChangeX, -fractionChangeY, true);
    }
    lastMousePos.x = currentMousePos.x;
    lastMousePos.y = currentMousePos.y;
}

void SwapBuffers(ES::Engine::Core &core) { glfwSwapBuffers(core.GetResource<Resource::ESGLFWWINDOW>().window); }

void PollEvents(ES::Engine::Core &core) { glfwPollEvents(); }

void LoadShaderManager(ES::Engine::Core &core)
{
    Resource::ShaderManager &shaderManager = core.RegisterResource<Resource::ShaderManager>(Resource::ShaderManager());
    Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"default"}, std::move(Utils::ShaderProgram()));
    sp.Create();
    sp.initFromFiles("shaders/simple.vert", "shaders/simple.frag");
}

void SetupShaderUniforms(ES::Engine::Core &core)
{
    auto &m_shaderProgram = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"default"});

    // Add uniforms
    m_shaderProgram.addUniform("MVP");
    m_shaderProgram.addUniform("ModelMatrix");  // View*Model : mat4
    m_shaderProgram.addUniform("NormalMatrix"); // Refer next slide : mat3

    for (int i = 0; i < 5; i++)
    {
        m_shaderProgram.addUniform("Light[" + std::to_string(i) + "].Position");
        m_shaderProgram.addUniform("Light[" + std::to_string(i) + "].Intensity");
    }
    m_shaderProgram.addUniform("Material.Ka");
    m_shaderProgram.addUniform("Material.Kd");
    m_shaderProgram.addUniform("Material.Ks");
    m_shaderProgram.addUniform("Material.Shiness");

    m_shaderProgram.addUniform("CamPos");
}

void LoadMaterialCache(ES::Engine::Core &core)
{
    auto &materialCache = core.RegisterResource<Resource::MaterialCache>({});
    materialCache.Add(entt::hashed_string("default"), std::move(Utils::Material()));
}

void CreateCamera(ES::Engine::Core &core)
{
    auto &camera = core.RegisterResource<Resource::Camera>(Resource::Camera(DEFAULT_WIDTH, DEFAULT_HEIGHT));
}

struct Light {
    glm::vec4 Position;
    glm::vec3 Intensity;
};

void UpdateMatrices(ES::Engine::Core &core)
{
    auto &cam = core.GetResource<Resource::Camera>();
    cam.view = glm::lookAt(cam.viewer.getViewPoint(), cam.viewer.getViewCenter(), cam.viewer.getUpVector());
    cam.projection = glm::perspective(glm::radians(45.0f), cam.size.x / cam.size.y, 0.1f, 100.0f);
}

void GLClearColor(ES::Engine::Core &core) { glClear(GL_COLOR_BUFFER_BIT); }

void GLClearDepth(ES::Engine::Core &core) { glClear(GL_DEPTH_BUFFER_BIT); }

void GLEnableDepth(ES::Engine::Core &core) { glEnable(GL_DEPTH_TEST); }

void GLEnableCullFace(ES::Engine::Core &core)
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void SetupLights(ES::Engine::Core &core)
{
    auto &shader = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"default"});

    Light light[] = {
        {glm::vec4(0, 0, 0, 1), glm::vec3(0.0f, 0.8f, 0.8f)},
        {glm::vec4(0, 0, 0, 1), glm::vec3(0.0f, 0.0f, 0.8f)},
        {glm::vec4(0, 0, 0, 1), glm::vec3(0.8f, 0.0f, 0.0f)},
        {glm::vec4(0, 0, 0, 1), glm::vec3(0.0f, 0.8f, 0.0f)},
        {glm::vec4(0, 0, 0, 1), glm::vec3(0.8f, 0.8f, 0.8f)}
    };

    float nbr_lights = 5.f;
    float scale = 2.f * glm::pi<float>() / nbr_lights;

    light[0].Position = glm::vec4(5.f * cosf(scale * 0.f), 5.f, 5.f * sinf(scale * 0.f), 1.f);
    light[1].Position = glm::vec4(5.f * cosf(scale * 1.f), 5.f, 5.f * sinf(scale * 1.f), 1.f);
    light[2].Position = glm::vec4(5.f * cosf(scale * 2.f), 5.f, 5.f * sinf(scale * 2.f), 1.f);
    light[3].Position = glm::vec4(5.f * cosf(scale * 3.f), 5.f, 5.f * sinf(scale * 3.f), 1.f);
    light[4].Position = glm::vec4(5.f * cosf(scale * 4.f), 5.f, 5.f * sinf(scale * 4.f), 1.f);

    shader.use();
    for (int i = 0; i < 5; i++)
    {
        glUniform4fv(shader.uniform("Light[" + std::to_string(i) + "].Position"), 1, glm::value_ptr(light[i].Position));
        glUniform3fv(shader.uniform("Light[" + std::to_string(i) + "].Intensity"), 1,
                     glm::value_ptr(light[i].Intensity));
    }
    shader.disable();
}

void SetupCamera(ES::Engine::Core &core)
{
    auto &shaderProgram = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"default"});
    shaderProgram.use();
    glUniform3fv(shaderProgram.uniform("CamPos"), 1,
                 glm::value_ptr(core.GetResource<Resource::Camera>().viewer.getViewPoint()));
    shaderProgram.disable();
}

void RenderMeshes(ES::Engine::Core &core)
{
    auto &view = core.GetResource<Resource::Camera>().view;
    auto &projection = core.GetResource<Resource::Camera>().projection;
    core.GetRegistry().view<Component::Model, ES::Plugin::Object::Component::Transform>().each(
        [&](auto entity, Component::Model &model, ES::Plugin::Object::Component::Transform &transform) {
            auto &shader =
                core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{model.shaderName.c_str()});
            const auto material =
                core.GetResource<Resource::MaterialCache>().Get(entt::hashed_string{model.materialName.c_str()});
            shader.use();
            glUniform3fv(shader.uniform("Material.Ka"), 1, glm::value_ptr(material.Ka));
            glUniform3fv(shader.uniform("Material.Kd"), 1, glm::value_ptr(material.Kd));
            glUniform3fv(shader.uniform("Material.Ks"), 1, glm::value_ptr(material.Ks));
            glUniform1fv(shader.uniform("Material.Shiness"), 1, &material.Shiness);
            glm::mat4 modelmat = transform.getTransformationMatrix();
            glm::mat4 mview = view * modelmat;
            glm::mat4 mvp = projection * view * modelmat;
            glm::mat4 imvp = glm::inverse(modelmat);
            glm::mat3 nmat = glm::mat3(glm::transpose(imvp)); // normal matrix
            glUniformMatrix3fv(shader.uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nmat));
            glUniformMatrix4fv(shader.uniform("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelmat));
            glUniformMatrix4fv(shader.uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
            model.mesh.draw();
            shader.disable();
        });
}
} // namespace ES::Plugin::ESGL::System
