#pragma once

#include "Core.hpp"

namespace ES::Plugin::OpenGL::System {
const int DEFAULT_WIDTH = 800;
const int DEFAULT_HEIGHT = 800;

void RenderMeshes(ES::Engine::Core &core);
void RenderText(ES::Engine::Core &core);
void RenderSprites(ES::Engine::Core &core);

void CreateCamera(ES::Engine::Core &core);
void LoadMaterialCache(ES::Engine::Core &core);
void UpdateMatrices(ES::Engine::Core &core);
void SetupLights(ES::Engine::Core &core);
void SetupCamera(ES::Engine::Core &core);
} // namespace ES::Plugin::OpenGL::System
