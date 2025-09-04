#include "OpenGL.pch.hpp"

#include "resource/CubeMapManager.hpp"
#include "resource/FontManager.hpp"
#include "resource/GLMeshBufferManager.hpp"
#include "resource/GLSpriteBufferManager.hpp"
#include "resource/GLTextBufferManager.hpp"
#include "system/ManagerSystems.hpp"
#include "resource/ShaderManager.hpp"
#include "resource/TextureManager.hpp"

void ES::Plugin::OpenGL::System::LoadFontManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::FontManager>(Resource::FontManager());
}

void ES::Plugin::OpenGL::System::LoadTextureManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::TextureManager>(Resource::TextureManager());
}

void ES::Plugin::OpenGL::System::LoadCubeMapManager(ES::Engine::Core &core)
{
    core.RegisterResource<Resource::CubeMapManager>(Resource::CubeMapManager());
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
