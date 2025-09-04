#include "OpenGL.pch.hpp"

#include "BufferSystems.hpp"
#include "resource/DirectionalLight.hpp"
#include "resource/GLMeshBufferManager.hpp"
#include "resource/GLSpriteBufferManager.hpp"
#include "resource/GLTextBufferManager.hpp"
#include "resource/TextureManager.hpp"
#include "component/ModelHandle.hpp"
#include "component/SpriteHandle.hpp"
#include "component/TextHandle.hpp"

void ES::Plugin::OpenGL::System::LoadGLSpriteBuffer(ES::Engine::Core &core)
{
    auto &glBufferManager = core.GetResource<Resource::GLSpriteBufferManager>();

    core.GetRegistry().view<Component::SpriteHandle, Component::Sprite>().each(
        [&]([[maybe_unused]] auto entity, Component::SpriteHandle &spriteHandle, Component::Sprite &sprite) {
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

void ES::Plugin::OpenGL::System::LoadGLMeshBuffer(ES::Engine::Core &core)
{
    auto &glBufferManager = core.GetResource<Resource::GLMeshBufferManager>();

    core.GetRegistry().view<Component::ModelHandle, ES::Plugin::Object::Component::Mesh>().each(
        [&]([[maybe_unused]] auto entity, Component::ModelHandle &model, ES::Plugin::Object::Component::Mesh &mesh) {
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

    core.GetRegistry().view<Component::TextHandle>().each(
        [&]([[maybe_unused]] auto entity, Component::TextHandle &textHandle) {
            if (glBufferManager.Contains(textHandle.id))
            {
                return;
            }
            Utils::GLTextBuffer buffer;
            buffer.GenerateGLTextBuffers();
            glBufferManager.Add(textHandle.id, std::move(buffer));
        });
}

void ES::Plugin::OpenGL::System::GenerateDirectionalLightFramebuffer(ES::Engine::Core &core)
{
    auto &light = core.GetResource<ES::Plugin::OpenGL::Resource::DirectionalLight>();

    glGenFramebuffers(1, &light.depthMapFBO);
}

void ES::Plugin::OpenGL::System::GenerateDirectionalLightTexture(ES::Engine::Core &core)
{
    auto &light = core.GetResource<ES::Plugin::OpenGL::Resource::DirectionalLight>();

    // Create a texture for the shadow map
    glGenTextures(1, &light.depthMap);

    // Setup the texture
    glBindTexture(GL_TEXTURE_2D, light.depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, light.shadowWidth, light.shadowHeight, 0, GL_DEPTH_COMPONENT,
                 GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Register the texture in the TextureManager, this is only for debug purposes
    core.GetResource<ES::Plugin::OpenGL::Resource::TextureManager>().Add(
        entt::hashed_string{"depthMap"}, light.shadowWidth, light.shadowHeight, 1, light.depthMap);
}

void ES::Plugin::OpenGL::System::BindDirectionalLightTextureToFramebuffer(ES::Engine::Core &core)
{
    const auto &light = core.GetResource<ES::Plugin::OpenGL::Resource::DirectionalLight>();

    // Attach the texture to the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, light.depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light.depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
