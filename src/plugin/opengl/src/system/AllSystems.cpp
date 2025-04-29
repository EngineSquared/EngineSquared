#include "AllSystems.hpp"

#include "Camera.hpp"
#include "Entity.hpp"
#include "Font.hpp"
#include "FontHandle.hpp"
#include "FontManager.hpp"
#include "GLMeshBufferManager.hpp"
#include "GLSpriteBufferManager.hpp"
#include "GLTextBufferManager.hpp"
#include "Input.hpp"
#include "Light.hpp"
#include "LightInfo.hpp"
#include "MaterialCache.hpp"
#include "MaterialHandle.hpp"
#include "Mesh.hpp"
#include "ModelHandle.hpp"
#include "MouseDragging.hpp"
#include "ShaderHandle.hpp"
#include "ShaderManager.hpp"
#include "Sprite.hpp"
#include "SpriteHandle.hpp"
#include "Text.hpp"
#include "TextHandle.hpp"
#include "TextureHandle.hpp"
#include "TextureManager.hpp"
#include "Window.hpp"

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

        uniform int NumberLights;

        struct LightInfo {
            vec4 Position;      // Light position (x, y, z) + w (Type of light)
            vec4 Colour;        // Light colour (x, y, z) + w (Intensity)
        };

        layout(std140, binding = 0) buffer LightBuffer {
            LightInfo Light[];
        };

        struct MaterialInfo {
            vec3 Ka; // Ambient reflectivity
            vec3 Kd; // Diffuse reflectivity
            vec3 Ks; // Specular reflectivity
            float Shiness; // Specular shininess factor (phong exponent)
        };
        uniform MaterialInfo Material;

        out vec4 FragColor;

        void main() {
            vec3 finalColor = vec3(0.0, 0.0, 0.0);
            vec3 ambient = vec3(0.0, 0.0, 0.0);

            for (int i = 0; i < NumberLights; i++) {
                int type = int(Light[i].Position.w);

                if (type == 0) { // Point light
                    vec3 L = normalize(Light[i].Position.xyz - Position);
                    vec3 V = normalize(CamPos - Position);
                    vec3 HalfwayVector = normalize(V + L);

                    vec3 diffuse = Material.Kd * Light[i].Colour.rgb * max(dot(L, Normal), 0.0);
                    vec3 specular = Material.Ks * Light[i].Colour.rgb * pow(max(dot(HalfwayVector, Normal), 0.0), Material.Shiness);
                    finalColor += diffuse + specular;
                } else if (type == 1) { // Ambient light
                    ambient += Material.Ka * Light[i].Colour.rgb;
                }
            }

            FragColor = vec4(finalColor + ambient, 1.0);
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
    Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"textDefault"});
    sp.Create();
    sp.initFromStrings(vertexShader, fragmentShader);
}

void ES::Plugin::OpenGL::System::LoadDefaultSpriteShader(ES::Engine::Core &core)
{
    const char *vertexShader = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;

        uniform mat4 model;
        uniform mat4 projection;


        void main()
        {
            // gl_Position = vec4(aPos, 1.0) + vec4(0.5, 0.5, 0.0, 0.0);
            gl_Position = projection * model * vec4(aPos, 1.0);
        }
    )";

    const char *fragmentShader = R"(
        #version 330 core
        out vec4 FragColor;
        uniform vec4 color;
        void main()
        {
            FragColor = color;
        }
    )";

    auto &shaderManager = core.GetResource<Resource::ShaderManager>();
    Utils::ShaderProgram &sp = shaderManager.Add(entt::hashed_string{"2DDefault"});
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

    m_shaderProgram.addUniform("NumberLights");
    m_shaderProgram.addSSBO("LightBuffer", 0, sizeof(ES::Plugin::OpenGL::Utils::LightInfo));
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

void ES::Plugin::OpenGL::System::SetupSpriteShaderUniforms(ES::Engine::Core &core)
{
    auto &m_shaderProgram = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"2DDefault"});

    m_shaderProgram.addUniform("color");
    m_shaderProgram.addUniform("model");
    m_shaderProgram.addUniform("projection");
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

void ES::Plugin::OpenGL::System::LoadTextureManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::TextureManager>(Resource::TextureManager());
}

void ES::Plugin::OpenGL::System::LoadGLSpriteBufferManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::GLSpriteBufferManager>(Resource::GLSpriteBufferManager());
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

void ES::Plugin::OpenGL::System::LoadGLSpriteBuffer(ES::Engine::Core &core)
{
    auto &glBufferManager = core.GetResource<Resource::GLSpriteBufferManager>();

    core.GetRegistry().view<Component::SpriteHandle, Component::Sprite>().each(
        [&](auto entity, Component::SpriteHandle &spriteHandle, Component::Sprite &sprite) {
            if (glBufferManager.Contains(spriteHandle.id))
            {
                glBufferManager.Get(spriteHandle.id).Update(sprite);
                return;
            }
            Utils::GLSpriteBuffer buffer;
            buffer.GenerateGLSpriteBuffers(sprite);
            glBufferManager.Add(spriteHandle.id, std::move(buffer));
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
    std::unordered_map<Component::ShaderHandle, std::vector<ES::Plugin::OpenGL::Utils::LightInfo>> ssbo_lights;

    core.GetRegistry().view<ES::Plugin::Object::Component::Transform, Component::ShaderHandle, Component::Light>().each(
        [&](auto entity, ES::Plugin::Object::Component::Transform &transform, Component::ShaderHandle &shaderHandle,
            ES::Plugin::OpenGL::Component::Light &light) {
            ES::Plugin::OpenGL::Utils::LightInfo lightInfo;
            lightInfo.position = glm::vec4(transform.position, static_cast<float>(light.type));
            lightInfo.colour = glm::vec4(light.colour, light.intensity);

            ssbo_lights[shaderHandle].emplace_back(lightInfo);
        });

    for (auto &[shaderId, lights] : ssbo_lights)
    {
        auto &shader = core.GetResource<Resource::ShaderManager>().Get(shaderId.id);
        shader.use();
        shader.updateSSBO("LightBuffer", lights.size() * sizeof(ES::Plugin::OpenGL::Utils::LightInfo),
                          static_cast<const void *>(lights.data()));
        glUniform1i(shader.uniform("NumberLights"), static_cast<int>(lights.size()));
        shader.disable();
    }
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

static void BindTextureIfNeeded(ES::Engine::Core &core, ES::Engine::Entity entity)
{
    ES::Plugin::OpenGL::Component::TextureHandle *textureHandle =
        ES::Engine::Entity(entity).TryGetComponent<ES::Plugin::OpenGL::Component::TextureHandle>(core);
    if (textureHandle)
        core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>().Get(textureHandle->id).Bind();
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
            BindTextureIfNeeded(core, entity);
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
            glUniform3f(shader.uniform("TextColor"), text.color.red, text.color.green, text.color.blue);

            auto &textBuffer = core.GetResource<Resource::GLTextBufferManager>().Get(textHandle.id);

            textBuffer.RenderText(text, font);

            shader.disable();
        });
}

void ES::Plugin::OpenGL::System::RenderSprites(ES::Engine::Core &core)
{
    auto &view = core.GetResource<Resource::Camera>().view;

    auto &size = core.GetResource<Resource::Camera>().size;

    glm::mat4 projection = glm::ortho(0.0f, size.x, 0.f, size.y, -1.0f, 1.0f);

    core.GetRegistry()
        .view<Component::Sprite, ES::Plugin::Object::Component::Transform, Component::ShaderHandle,
              Component::SpriteHandle>()
        .each([&](auto e, Component::Sprite &sprite, ES::Plugin::Object::Component::Transform &transform,
                  Component::ShaderHandle &shaderHandle, Component::SpriteHandle &spriteHandle) {
            auto &shader = core.GetResource<Resource::ShaderManager>().Get(shaderHandle.id);
            const auto &glBuffer = core.GetResource<Resource::GLSpriteBufferManager>().Get(spriteHandle.id);

            shader.use();

            glUniform4f(shader.uniform("color"), sprite.color.red, sprite.color.green, sprite.color.blue,
                        sprite.color.alpha);
            glUniformMatrix4fv(shader.uniform("model"), 1, GL_FALSE,
                               glm::value_ptr(transform.getTransformationMatrix()));
            glUniformMatrix4fv(shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));

            Component::TextureHandle *textureHandle =
                ES::Engine::Entity(e).TryGetComponent<Component::TextureHandle>(core);
            if (textureHandle)
                core.GetResource<Resource::TextureManager>().Get(textureHandle->id).Bind();

            glBuffer.Draw();
            shader.disable();
        });
}
