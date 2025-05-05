#include "RenderSystems.hpp"
#include "Camera.hpp"
#include "Entity.hpp"
#include "FontHandle.hpp"
#include "FontManager.hpp"
#include "GLMeshBufferManager.hpp"
#include "GLSpriteBufferManager.hpp"
#include "GLTextBufferManager.hpp"
#include "Light.hpp"
#include "MaterialCache.hpp"
#include "MaterialHandle.hpp"
#include "ModelHandle.hpp"
#include "Object.hpp"
#include "ShaderHandle.hpp"
#include "ShaderManager.hpp"
#include "Sprite.hpp"
#include "SpriteHandle.hpp"
#include "Text.hpp"
#include "TextHandle.hpp"
#include "TextureHandle.hpp"
#include "TextureManager.hpp"

#include <glm/gtc/type_ptr.hpp>

static void BindTextureIfNeeded(ES::Engine::Core &core, ES::Engine::Entity entity)
{
    ES::Plugin::OpenGL::Component::TextureHandle *textureHandle =
        ES::Engine::Entity(entity).TryGetComponent<ES::Plugin::OpenGL::Component::TextureHandle>(core);
    if (textureHandle)
        core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>().Get(textureHandle->id).Bind();
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
