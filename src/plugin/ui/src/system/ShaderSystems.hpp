#pragma once

#include "core/Core.hpp"

namespace ES::Plugin::UI::System {
void LoadShaderVertColor(ES::Engine::Core &core);
void LoadShaderVertTexture(ES::Engine::Core &core);
void LoadShaderVertGradient(ES::Engine::Core &core);
void LoadShaderCreation(ES::Engine::Core &core);
void LoadShaderPassthrough(ES::Engine::Core &core);

void LoadShaderColorMatrix(ES::Engine::Core &core);
void LoadShaderBlendMatrix(ES::Engine::Core &core);
void LoadShaderBlur(ES::Engine::Core &core);
void LoadShaderDropShadow(ES::Engine::Core &core);

void SetupShaderVertColorUniforms(ES::Engine::Core &core);
void SetupShaderVertTextureUniforms(ES::Engine::Core &core);
void SetupShaderVertGradientUniforms(ES::Engine::Core &core);
void SetupShaderCreationUniforms(ES::Engine::Core &core);
void SetupShaderPassthroughUniform(ES::Engine::Core &core);
void SetupShaderColorMatrix(ES::Engine::Core &core);
void SetupShaderBlendMatrix(ES::Engine::Core &core);
void SetupShaderBlur(ES::Engine::Core &core);
void SetupShaderDropShadow(ES::Engine::Core &core);
} // namespace ES::Plugin::UI::System
