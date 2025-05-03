#pragma once

#include "Core.hpp"

namespace ES::Plugin::OpenGL::System {
const int DEFAULT_WIDTH = 800;
const int DEFAULT_HEIGHT = 800;

void InitGLEW(const ES::Engine::Core &core);
void CheckGLEWVersion(const ES::Engine::Core &core);
void SetupResizeViewport(ES::Engine::Core &core);
void SetupMouseDragging(ES::Engine::Core &core);
void CreateCamera(ES::Engine::Core &core);

void LoadMaterialCache(ES::Engine::Core &core);
void UpdateMatrices(ES::Engine::Core &core);
void SetupLights(ES::Engine::Core &core);
void SetupCamera(ES::Engine::Core &core);

void GLClearColor(const ES::Engine::Core &core);
void GLClearDepth(const ES::Engine::Core &core);
void GLEnableDepth(const ES::Engine::Core &core);
void GLEnableCullFace(const ES::Engine::Core &core);
} // namespace ES::Plugin::OpenGL::System
