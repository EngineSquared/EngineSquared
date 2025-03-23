#include "AllSystems.hpp"

#include "Entity.hpp"
#include "GLBufferManager.hpp"
#include "Material.hpp"
#include "MaterialCache.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "ShaderManager.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Buttons.hpp"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>

void ES::Plugin::OpenGL::System::InitGLEW(const ES::Engine::Core &)
{
    if (GLenum err = glewInit(); GLEW_OK != err)
    {
        std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;
}

void ES::Plugin::OpenGL::System::CheckGLEWVersion(const ES::Engine::Core &)
{
    if (!glewIsSupported("GL_VERSION_4_2"))
    {
        std::cerr << "OpenGL 4.2 not supported" << std::endl;
        return;
    }
    std::cout << "OpenGL 4.2 supported" << std::endl;
}

// Function to handle mouse dragging interactions
void ES::Plugin::OpenGL::System::MouseDragging(ES::Engine::Core &core)
{
    auto &buttons = core.GetResource<Window::Resource::Buttons>();
    auto &lastMousePos = buttons.lastMousePos;
    auto &currentMousePos = buttons.currentMousePos;
    auto &mouseButtons = buttons.mouse;
    auto &camera = core.GetResource<Resource::Camera>();
    if (mouseButtons[GLFW_MOUSE_BUTTON_LEFT].pressed)
    {
        float fractionChangeX = static_cast<float>(currentMousePos.x - lastMousePos.x) / camera.size.x;
        float fractionChangeY = static_cast<float>(lastMousePos.y - currentMousePos.y) / camera.size.y;
        camera.viewer.rotate(fractionChangeX, fractionChangeY);
    }
    else if (mouseButtons[GLFW_MOUSE_BUTTON_MIDDLE].pressed)
    {
        float fractionChangeY = static_cast<float>(lastMousePos.y - currentMousePos.y) / camera.size.y;
        camera.viewer.zoom(fractionChangeY);
    }
    else if (mouseButtons[GLFW_MOUSE_BUTTON_RIGHT].pressed)
    {
        float fractionChangeX = static_cast<float>(currentMousePos.x - lastMousePos.x) / camera.size.x;
        float fractionChangeY = static_cast<float>(lastMousePos.y - currentMousePos.y) / camera.size.y;
        camera.viewer.translate(-fractionChangeX, -fractionChangeY, true);
    }
    lastMousePos.x = currentMousePos.x;
    lastMousePos.y = currentMousePos.y;
}

void ES::Plugin::OpenGL::System::LoadShaderManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::ShaderManager>(Resource::ShaderManager());
}

void ES::Plugin::OpenGL::System::LoadDefaultShader(ES::Engine::Core &core)
{
    const char *vertexShader = R"(
        #version 440

        layout (location = 0) in vec4 VertexPosition;
        layout (location = 1) in vec3 VertexNormal;

        out vec3 Position;
        out vec3 Normal;

        uniform mat4 ModelMatrix;
        uniform mat3 NormalMatrix;
        uniform mat4 MVP;

        void main()
        {
            Normal = normalize(NormalMatrix * VertexNormal);
            Position = (ModelMatrix * VertexPosition).xyz;
            gl_Position = MVP * VertexPosition;
        }
    )";

    const char *fragmentShader = R"(
        #version 440

        in vec3 Position;
        in vec3 Normal;

        uniform vec3 CamPos;

        struct LightInfo {
            vec4 Position; // Light position in eye coords.
            vec3 Intensity; // Light intensity
        };
        uniform LightInfo Light[5];

        struct MaterialInfo {
            vec3 Ka; // Ambient reflectivity
            vec3 Kd; // Diffuse reflectivity
            vec3 Ks; // Specular reflectivity
            float Shiness; // Specular shininess factor (phong exponent)
        };
        uniform MaterialInfo Material;

        out vec4 FragColor;

        void main() {
            vec3 finalColor = vec3(0,0,0);
            vec3 ambient = Material.Ka * Light[0].Intensity;
            for (int i = 0; i < 4; i++) {
                vec3 L = normalize(Light[i].Position.xyz - Position);
                vec3 V = normalize(CamPos - Position);
                vec3 diffuse = Material.Kd * Light[i].Intensity * max( dot(L, Normal), 0.0);
                vec3 HalfwayVector = normalize(V + L);
                vec3 specular = Material.Ks * Light[i].Intensity * pow( max( dot( HalfwayVector, Normal), 0.0), Material.Shiness);
                finalColor = finalColor + diffuse + specular;
            }
            vec3 L = normalize(Light[4].Position.xyz);
            vec3 V = normalize(CamPos - Position);
            vec3 diffuse = Material.Kd * Light[4].Intensity * max( dot(L, Normal), 0.0);
            vec3 HalfwayVector = normalize(V + L);
            vec3 specular = Material.Ks * Light[4].Intensity * pow( max( dot( HalfwayVector, Normal), 0.0), Material.Shiness);
            finalColor = finalColor + diffuse + specular;
            finalColor = ambient + finalColor;
            FragColor = vec4(finalColor, 1.0);
        }
    )";

    auto &shaderManager = core.GetResource<Resource::ShaderManager>();
    Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"default"}, std::move(Utils::ShaderProgram()));
    sp.Create();
    sp.initFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::OpenGL::System::SetupShaderUniforms(ES::Engine::Core &core)
{
    auto &m_shaderProgram = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"default"});

    // Add uniforms
    m_shaderProgram.addUniform("MVP");
    m_shaderProgram.addUniform("ModelMatrix");  // View*Model : mat4
    m_shaderProgram.addUniform("NormalMatrix"); // Refer next slide : mat3

    for (int i = 0; i < 5; i++)
    {
        m_shaderProgram.addUniform(fmt::format("Light[{}].Position", i));
        m_shaderProgram.addUniform(fmt::format("Light[{}].Intensity", i));
    }
    m_shaderProgram.addUniform("Material.Ka");
    m_shaderProgram.addUniform("Material.Kd");
    m_shaderProgram.addUniform("Material.Ks");
    m_shaderProgram.addUniform("Material.Shiness");

    m_shaderProgram.addUniform("CamPos");
}

void ES::Plugin::OpenGL::System::LoadMaterialCache(ES::Engine::Core &core)
{
    auto &materialCache = core.RegisterResource<Resource::MaterialCache>({});
    materialCache.Add(entt::hashed_string("default"), std::move(Utils::MaterialData()));
}

void ES::Plugin::OpenGL::System::LoadGLBufferManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::GLBufferManager>(Resource::GLBufferManager());
}

void ES::Plugin::OpenGL::System::LoadGLBuffer(ES::Engine::Core &core)
{
    auto &glBufferManager = core.GetResource<Resource::GLBufferManager>();

    core.GetRegistry().view<Component::Model, ES::Plugin::Object::Component::Mesh>().each(
        [&](auto entity, Component::Model &model, ES::Plugin::Object::Component::Mesh &mesh) {
            if (glBufferManager.Contains(entt::hashed_string(model.name.c_str())))
            {
                glBufferManager.Get(entt::hashed_string{model.name.c_str()}).update(mesh);
                return;
            }
            Utils::GLBuffer buffer;
            buffer.generateGlBuffers(mesh);
            glBufferManager.Add(entt::hashed_string(model.name.c_str()), std::move(buffer));
        });
}

void ES::Plugin::OpenGL::System::CreateCamera(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::Camera>(Resource::Camera(DEFAULT_WIDTH, DEFAULT_HEIGHT));
}

void ES::Plugin::OpenGL::System::UpdateMatrices(ES::Engine::Core &core)
{
    auto &cam = core.GetResource<Resource::Camera>();
    cam.view = glm::lookAt(cam.viewer.getViewPoint(), cam.viewer.getViewCenter(), cam.viewer.getUpVector());
    cam.projection = glm::perspective(glm::radians(45.0f), cam.size.x / cam.size.y, 0.1f, 100.0f);
}

void ES::Plugin::OpenGL::System::GLClearColor(const ES::Engine::Core &) { glClear(GL_COLOR_BUFFER_BIT); }

void ES::Plugin::OpenGL::System::GLClearDepth(const ES::Engine::Core &) { glClear(GL_DEPTH_BUFFER_BIT); }

void ES::Plugin::OpenGL::System::GLEnableDepth(const ES::Engine::Core &) { glEnable(GL_DEPTH_TEST); }

void ES::Plugin::OpenGL::System::GLEnableCullFace(const ES::Engine::Core &)
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
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

static void LoadMaterial(ES::Plugin::OpenGL::Utils::ShaderProgram &shaderProgram,
                         const ES::Plugin::OpenGL::Utils::MaterialData &material)
{
    glUniform3fv(shaderProgram.uniform("Material.Ka"), 1, glm::value_ptr(material.Ka));
    glUniform3fv(shaderProgram.uniform("Material.Kd"), 1, glm::value_ptr(material.Kd));
    glUniform3fv(shaderProgram.uniform("Material.Ks"), 1, glm::value_ptr(material.Ks));
    glUniform1fv(shaderProgram.uniform("Material.Shiness"), 1, &material.Shiness);
}

void ES::Plugin::OpenGL::System::RenderMeshes(ES::Engine::Core &core)
{
    auto &view = core.GetResource<Resource::Camera>().view;
    auto &projection = core.GetResource<Resource::Camera>().projection;
    core.GetRegistry()
        .view<Component::Model, ES::Plugin::Object::Component::Transform, ES::Plugin::Object::Component::Mesh, Component::Shader, Component::Material>()
        .each([&](auto entity, Component::Model &model, ES::Plugin::Object::Component::Transform &transform,
                  ES::Plugin::Object::Component::Mesh &mesh, Component::Shader &shader, Component::Material &material) {
            auto &shaderProgram =
                core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{shader.name.c_str()});
            const auto &materialData =
                core.GetResource<Resource::MaterialCache>().Get(entt::hashed_string{material.name.c_str()});
            const auto &glBuffer =
                core.GetResource<Resource::GLBufferManager>().Get(entt::hashed_string{model.name.c_str()});
                shaderProgram.use();
            LoadMaterial(shaderProgram, materialData);
            glm::mat4 modelmat = transform.getTransformationMatrix();
            glm::mat4 mview = view * modelmat;
            glm::mat4 mvp = projection * view * modelmat;
            auto nmat = glm::mat3(glm::transpose(glm::inverse(modelmat))); // normal matrix
            glUniformMatrix3fv(shaderProgram.uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nmat));
            glUniformMatrix4fv(shaderProgram.uniform("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelmat));
            glUniformMatrix4fv(shaderProgram.uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
            glBuffer.draw(mesh);
            shaderProgram.disable();
        });
}
