#include "ManagerSystems.hpp"
#include "FontManager.hpp"
#include "GLMeshBufferManager.hpp"
#include "GLSpriteBufferManager.hpp"
#include "GLTextBufferManager.hpp"
#include "ShaderManager.hpp"
#include "TextureManager.hpp"

void ES::Plugin::OpenGL::System::LoadFontManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::FontManager>(Resource::FontManager());
}

void ES::Plugin::OpenGL::System::LoadTextureManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::TextureManager>(Resource::TextureManager());
}

void ES::Plugin::OpenGL::System::LoadGLMeshBufferManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::GLMeshBufferManager>(Resource::GLMeshBufferManager());
}

void ES::Plugin::OpenGL::System::LoadGLTextBufferManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::GLTextBufferManager>(Resource::GLTextBufferManager());
}

void ES::Plugin::OpenGL::System::LoadGLSpriteBufferManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::GLSpriteBufferManager>(Resource::GLSpriteBufferManager());
}

void ES::Plugin::OpenGL::System::LoadShaderManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::ShaderManager>(Resource::ShaderManager());
}
