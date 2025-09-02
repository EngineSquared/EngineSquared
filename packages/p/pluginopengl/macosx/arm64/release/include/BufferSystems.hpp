#pragma once

#include "Core.hpp"

namespace ES::Plugin::OpenGL::System {
void LoadGLMeshBuffer(ES::Engine::Core &core);
void LoadGLSpriteBuffer(ES::Engine::Core &core);
void LoadGLTextBuffer(ES::Engine::Core &core);

void GenerateDirectionalLightFramebuffer(ES::Engine::Core &core);
void GenerateDirectionalLightTexture(ES::Engine::Core &core);
void BindDirectionalLightTextureToFramebuffer(ES::Engine::Core &core);
} // namespace ES::Plugin::OpenGL::System
