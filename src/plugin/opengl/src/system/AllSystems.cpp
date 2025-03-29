#include "AllSystems.hpp"

#include "Buttons.hpp"
#include "Camera.hpp"
#include "Entity.hpp"
#include "Font.hpp"
#include "FontHandle.hpp"
#include "FontManager.hpp"
#include "GLMeshBufferManager.hpp"
#include "GLTextBufferManager.hpp"
#include "Light.hpp"
#include "MaterialCache.hpp"
#include "MaterialHandle.hpp"
#include "Mesh.hpp"
#include "ModelHandle.hpp"
#include "ShaderHandle.hpp"
#include "ShaderManager.hpp"
#include "Text.hpp"
#include "TextHandle.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>

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
    ES::Utils::Log::Info("OpenGL 4.2 supported");
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

void ES::Plugin::OpenGL::System::LoadFontManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::FontManager>(Resource::FontManager());
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
    Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"default"});
    sp.Create();
    sp.initFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::OpenGL::System::LoadDefaultTextShader(ES::Engine::Core &core)
{
    const char *vertexShader = R"(
        #version 440
        layout (location = 0) in vec4 vertex;

        out vec2 TexCoords;

        uniform mat4 Projection;

        void main() {
            gl_Position = Projection * vec4(vertex.xy, 0.0, 1.0);
            TexCoords = vertex.zw;
        }
    )";

    const char *fragmentShader = R"(
        #version 440 core
        in vec2 TexCoords;
        out vec4 FragColor;

        uniform sampler2D Text;
        uniform vec3 TextColor;

        void main() {
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(Text, TexCoords).r);
            FragColor = vec4(TextColor, 1.0) * sampled;
        }
    )";

    auto &shaderManager = core.GetResource<Resource::ShaderManager>();
    Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"textDefault"}, std::move(Utils::ShaderProgram()));
    sp.Create();
    sp.initFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::OpenGL::System::LoadDefaultTextShader(ES::Engine::Core &core)
{
    const char *vertexShader = R"(
        #version 440
        layout (location = 0) in vec4 vertex;

        out vec2 TexCoords;

        uniform mat4 Projection;

        void main() {
            gl_Position = Projection * vec4(vertex.xy, 0.0, 1.0);
            TexCoords = vertex.zw;
        }
    )";

    const char *fragmentShader = R"(
        #version 440 core
        in vec2 TexCoords;
        out vec4 FragColor;

        uniform sampler2D Text;
        uniform vec3 TextColor;

        void main() {
            vec4 sampled = vec4(1.0, 1.0, 1.0, texture(Text, TexCoords).r);
            FragColor = vec4(TextColor, 1.0) * sampled;
        }
    )";

    auto &shaderManager = core.GetResource<Resource::ShaderManager>();
    Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"textDefault"}, std::move(Utils::ShaderProgram()));
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

void ES::Plugin::OpenGL::System::SetupTextShaderUniforms(ES::Engine::Core &core)
{
    auto &m_shaderProgram = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"textDefault"});

    m_shaderProgram.addUniform("Projection");
    m_shaderProgram.addUniform("Text");
    m_shaderProgram.addUniform("TextColor");
}

void ES::Plugin::OpenGL::System::LoadMaterialCache(ES::Engine::Core &core)
{
    auto &materialCache = core.RegisterResource<Resource::MaterialCache>({});
    materialCache.Add(entt::hashed_string("default"), std::move(Utils::Material()));
}

void ES::Plugin::OpenGL::System::LoadGLMeshBufferManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::GLMeshBufferManager>(Resource::GLMeshBufferManager());
}

void ES::Plugin::OpenGL::System::LoadGLTextBufferManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::GLTextBufferManager>(Resource::GLTextBufferManager());
}

void ES::Plugin::OpenGL::System::LoadGLMeshBuffer(ES::Engine::Core &core)
{
    auto &glBufferManager = core.GetResource<Resource::GLMeshBufferManager>();

    core.GetRegistry().view<Component::ModelHandle, ES::Plugin::Object::Component::Mesh>().each(
        [&](auto entity, Component::ModelHandle &model, ES::Plugin::Object::Component::Mesh &mesh) {
            if (glBufferManager.Contains(model.id))
            {
                glBufferManager.Get(model.id).Update(mesh);
                return;
            }
            Utils::GLMeshBuffer buffer;
            buffer.GenerateGLMeshBuffers(mesh);
            glBufferManager.Add(model.id, std::move(buffer));
        });
}

void ES::Plugin::OpenGL::System::LoadGLTextBuffer(ES::Engine::Core &core)
{
    auto &glBufferManager = core.GetResource<Resource::GLTextBufferManager>();

    core.GetRegistry().view<Component::TextHandle, ES::Plugin::UI::Component::Text>().each(
        [&](auto entity, Component::TextHandle &textHandle, ES::Plugin::UI::Component::Text &text) {
            if (glBufferManager.Contains(textHandle.id))
            {
                return;
            }
            Utils::GLTextBuffer buffer;
            buffer.GenerateGLTextBuffers();
            glBufferManager.Add(textHandle.id, std::move(buffer));
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

static void LoadMaterial(ES::Plugin::OpenGL::Utils::ShaderProgram &shader,
                         const ES::Plugin::OpenGL::Utils::Material &material)
{
    glUniform3fv(shader.uniform("Material.Ka"), 1, glm::value_ptr(material.Ka));
    glUniform3fv(shader.uniform("Material.Kd"), 1, glm::value_ptr(material.Kd));
    glUniform3fv(shader.uniform("Material.Ks"), 1, glm::value_ptr(material.Ks));
    glUniform1fv(shader.uniform("Material.Shiness"), 1, &material.Shiness);
}

void ES::Plugin::OpenGL::System::RenderMeshes(ES::Engine::Core &core)
{
    auto &view = core.GetResource<Resource::Camera>().view;
    auto &projection = core.GetResource<Resource::Camera>().projection;

    core.GetRegistry()
        .view<Component::ModelHandle, ES::Plugin::Object::Component::Transform, ES::Plugin::Object::Component::Mesh,
              Component::ShaderHandle, Component::MaterialHandle>()
        .each([&](auto entity, Component::ModelHandle &modelHandle, ES::Plugin::Object::Component::Transform &transform,
                  ES::Plugin::Object::Component::Mesh &mesh, Component::ShaderHandle &shaderHandle,
                  Component::MaterialHandle &materialHandle) {
            auto &shader = core.GetResource<Resource::ShaderManager>().Get(shaderHandle.id);
            const auto &material = core.GetResource<Resource::MaterialCache>().Get(materialHandle.id);
            const auto &glBuffer = core.GetResource<Resource::GLMeshBufferManager>().Get(modelHandle.id);
            shader.use();
            LoadMaterial(shader, material);
            glm::mat4 modelmat = transform.getTransformationMatrix();
            glm::mat4 mview = view * modelmat;
            glm::mat4 mvp = projection * view * modelmat;
            auto nmat = glm::mat3(glm::transpose(glm::inverse(modelmat))); // normal matrix
            glUniformMatrix3fv(shader.uniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nmat));
            glUniformMatrix4fv(shader.uniform("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelmat));
            glUniformMatrix4fv(shader.uniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
            glBuffer.Draw(mesh);
            shader.disable();
        });
}

void ES::Plugin::OpenGL::System::RenderText(ES::Engine::Core &core)
{
    auto &view = core.GetResource<Resource::Camera>().view;

    auto &size = core.GetResource<Resource::Camera>().size;

    glm::mat4 projection = glm::ortho(0.0f, size.x, 0.0f, size.y, -1.0f, 1.0f);

    core.GetRegistry()
        .view<ES::Plugin::UI::Component::Text, Component::FontHandle, Component::ShaderHandle, Component::TextHandle>()
        .each([&](auto entity, ES::Plugin::UI::Component::Text &text, Component::FontHandle &fontHandle,
                  Component::ShaderHandle &shaderHandle, Component::TextHandle &textHandle) {
            auto &shader = core.GetResource<Resource::ShaderManager>().Get(shaderHandle.id);
            const auto &font = core.GetResource<Resource::FontManager>().Get(fontHandle.id);

            shader.use();

            glUniformMatrix4fv(shader.uniform("Projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1i(shader.uniform("Text"), 0);
            glUniform3f(shader.uniform("TextColor"), text.color.x, text.color.y, text.color.z);

            auto &textBuffer = core.GetResource<Resource::GLTextBufferManager>().Get(textHandle.id);

            textBuffer.RenderText(text, font);

            shader.disable();
        });
}
