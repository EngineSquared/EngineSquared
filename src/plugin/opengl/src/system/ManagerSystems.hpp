#pragma once

#include "Core.hpp"

namespace ES::Plugin::OpenGL::System {
void LoadFontManager(ES::Engine::Core &core);
void LoadTextureManager(ES::Engine::Core &core);
void LoadGLMeshBufferManager(ES::Engine::Core &core);
void LoadGLSpriteBufferManager(ES::Engine::Core &core);
void LoadGLTextBufferManager(ES::Engine::Core &core);
void LoadShaderManager(ES::Engine::Core &core);
} // namespace ES::Plugin::OpenGL::System
