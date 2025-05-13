#include "RenderSystems.hpp"
#include "Camera.hpp"
#include "Entity.hpp"
#include "FontHandle.hpp"
#include "FontManager.hpp"
#include "GLMeshBufferManager.hpp"
#include "GLSpriteBufferManager.hpp"
#include "GLTextBufferManager.hpp"
#include "Light.hpp"
#include "LightInfo.hpp"
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
    glUniform3fv(shader.GetUniform("Material.Ka"), 1, glm::value_ptr(material.Ka));
    glUniform3fv(shader.GetUniform("Material.Kd"), 1, glm::value_ptr(material.Kd));
    glUniform3fv(shader.GetUniform("Material.Ks"), 1, glm::value_ptr(material.Ks));
    glUniform1fv(shader.GetUniform("Material.Shiness"), 1, &material.Shiness);
}

void ES::Plugin::OpenGL::System::RenderMeshes(ES::Engine::Core &core)
{
    auto &view = core.GetResource<Resource::Camera>().view;
    auto &projection = core.GetResource<Resource::Camera>().projection;

    core.GetRegistry()
        .view<Component::ModelHandle, ES::Plugin::Object::Component::Transform, ES::Plugin::Object::Component::Mesh,
              Component::MaterialHandle, Component::ShaderHandle>()
        .each([&](auto entity, Component::ModelHandle &modelHandle, ES::Plugin::Object::Component::Transform &transform,
                  ES::Plugin::Object::Component::Mesh &mesh, Component::MaterialHandle &materialHandle,
                  Component::ShaderHandle &shaderHandle) {
            auto &shader = core.GetResource<Resource::ShaderManager>().Get(shaderHandle.id);
            const auto &material = core.GetResource<Resource::MaterialCache>().Get(materialHandle.id);
            const auto &glBuffer = core.GetResource<Resource::GLMeshBufferManager>().Get(modelHandle.id);
            shader.Use();
            LoadMaterial(shader, material);
            glm::mat4 modelmat = transform.getTransformationMatrix();
            glm::mat4 mvp = projection * view * modelmat;
            auto nmat = glm::mat3(glm::transpose(glm::inverse(modelmat))); // normal matrix
            glUniformMatrix3fv(shader.GetUniform("NormalMatrix"), 1, GL_FALSE, glm::value_ptr(nmat));
            glUniformMatrix4fv(shader.GetUniform("ModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelmat));
            glUniformMatrix4fv(shader.GetUniform("MVP"), 1, GL_FALSE, glm::value_ptr(mvp));
            BindTextureIfNeeded(core, entity);
            glBuffer.Draw(mesh);
            shader.Disable();
        });
}

void ES::Plugin::OpenGL::System::RenderText(ES::Engine::Core &core)
{
    auto &size = core.GetResource<Resource::Camera>().size;

    glm::mat4 projection = glm::ortho(0.0f, size.x, 0.0f, size.y, -1.0f, 1.0f);

    core.GetRegistry()
        .view<ES::Plugin::UI::Component::Text, Component::TextHandle, Component::ShaderHandle, Component::FontHandle>()
        .each([&]([[maybe_unused]] auto entity, ES::Plugin::UI::Component::Text &text, Component::TextHandle &textHandle,
                  Component::ShaderHandle &shaderHandle, Component::FontHandle &fontHandle) {
            const auto &font = core.GetResource<Resource::FontManager>().Get(fontHandle.id);
            auto &shader = core.GetResource<Resource::ShaderManager>().Get(shaderHandle.id);

            shader.Use();

            glUniformMatrix4fv(shader.GetUniform("Projection"), 1, GL_FALSE, glm::value_ptr(projection));
            glUniform1i(shader.GetUniform("Text"), 0);
            glUniform3f(shader.GetUniform("TextColor"), text.color.red, text.color.green, text.color.blue);

            auto &textBuffer = core.GetResource<Resource::GLTextBufferManager>().Get(textHandle.id);

            textBuffer.RenderText(text, font);

            shader.Disable();
        });
}

void ES::Plugin::OpenGL::System::RenderSprites(ES::Engine::Core &core)
{
    auto &size = core.GetResource<Resource::Camera>().size;

    glm::mat4 projection = glm::ortho(0.0f, size.x, 0.f, size.y, -1.0f, 1.0f);

    core.GetRegistry()
        .view<Component::Sprite, ES::Plugin::Object::Component::Transform, Component::SpriteHandle,
              Component::ShaderHandle>()
        .each([&](auto entity, Component::Sprite &sprite, ES::Plugin::Object::Component::Transform &transform,
                  Component::SpriteHandle &spriteHandle, Component::ShaderHandle &shaderHandle) {
            const auto &glBuffer = core.GetResource<Resource::GLSpriteBufferManager>().Get(spriteHandle.id);
            auto &shader = core.GetResource<Resource::ShaderManager>().Get(shaderHandle.id);

            shader.Use();

            glUniform4f(shader.GetUniform("color"), sprite.color.red, sprite.color.green, sprite.color.blue,
                        sprite.color.alpha);
            glUniformMatrix4fv(shader.GetUniform("model"), 1, GL_FALSE,
                               glm::value_ptr(transform.getTransformationMatrix()));
            glUniformMatrix4fv(shader.GetUniform("projection"), 1, GL_FALSE, glm::value_ptr(projection));

            Component::TextureHandle *textureHandle =
                ES::Engine::Entity(entity).TryGetComponent<Component::TextureHandle>(core);
            if (textureHandle)
                core.GetResource<Resource::TextureManager>().Get(textureHandle->id).Bind();

            glBuffer.Draw();
            shader.Disable();
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
    std::unordered_map<Component::ShaderHandle, std::vector<ES::Plugin::OpenGL::Utils::LightInfo>> ssbo_lights;

    core.GetRegistry().view<ES::Plugin::Object::Component::Transform, Component::ShaderHandle, Component::Light>().each(
        [&]([[maybe_unused]] auto entity, ES::Plugin::Object::Component::Transform &transform, Component::ShaderHandle &shaderHandle,
            ES::Plugin::OpenGL::Component::Light &light) {
            ES::Plugin::OpenGL::Utils::LightInfo lightInfo;
            lightInfo.position = glm::vec4(transform.position, static_cast<float>(light.type));
            lightInfo.colour = glm::vec4(light.colour, light.intensity);

            ssbo_lights[shaderHandle].emplace_back(lightInfo);
        });

    for (auto &[shaderId, lights] : ssbo_lights)
    {
        auto &shader = core.GetResource<Resource::ShaderManager>().Get(shaderId.id);
        shader.Use();
        shader.UpdateSSBO("LightBuffer", lights.size() * sizeof(ES::Plugin::OpenGL::Utils::LightInfo),
                          static_cast<const GLvoid *>(lights.data()));
        glUniform1i(shader.GetUniform("NumberLights"), static_cast<int>(lights.size()));
        shader.Disable();
    }
}

void ES::Plugin::OpenGL::System::SetupCamera(ES::Engine::Core &core)
{
    auto &shaderProgram = core.GetResource<Resource::ShaderManager>().Get(entt::hashed_string{"default"});
    shaderProgram.Use();
    glUniform3fv(shaderProgram.GetUniform("CamPos"), 1,
                 glm::value_ptr(core.GetResource<Resource::Camera>().viewer.getViewPoint()));
    shaderProgram.Disable();
}
