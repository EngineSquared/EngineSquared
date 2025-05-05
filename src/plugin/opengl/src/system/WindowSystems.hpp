#pragma once

#include "Core.hpp"

namespace ES::Plugin::OpenGL::System {
void InitGLEW(const ES::Engine::Core &core);
void CheckGLEWVersion(const ES::Engine::Core &core);
void SetupResizeViewport(ES::Engine::Core &core);
void SetupMouseDragging(ES::Engine::Core &core);

void GLClearColor(const ES::Engine::Core &core);
void GLClearDepth(const ES::Engine::Core &core);
void GLEnableDepth(const ES::Engine::Core &core);
void GLEnableCullFace(const ES::Engine::Core &core);
} // namespace ES::Plugin::OpenGL::System
