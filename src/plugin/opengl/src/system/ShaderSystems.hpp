#pragma once

#include "Core.hpp"

namespace ES::Plugin::OpenGL::System {
void LoadDefaultShader(ES::Engine::Core &core);
void LoadDefaultTextShader(ES::Engine::Core &core);
void LoadDefaultSpriteShader(ES::Engine::Core &core);

void SetupShaderUniforms(ES::Engine::Core &core);
void SetupTextShaderUniforms(ES::Engine::Core &core);
void SetupSpriteShaderUniforms(ES::Engine::Core &core);
} // namespace ES::Plugin::OpenGL::System
