#include "BufferSystems.hpp"
#include "GLMeshBufferManager.hpp"
#include "GLSpriteBufferManager.hpp"
#include "GLTextBufferManager.hpp"
#include "ModelHandle.hpp"
#include "SpriteHandle.hpp"
#include "TextHandle.hpp"
#include "TextureManager.hpp"

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
