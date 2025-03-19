#pragma once

#include "Entity.hpp"
#include "GLFWWindow.hpp"
#include "Material.hpp"
#include "MaterialCache.hpp"
#include "Mesh.hpp"
#include "Shader.hpp"
#include "ShaderManager.hpp"
#include "resource/Buttons/Buttons.hpp"
#include "resource/Camera/Camera.hpp"
#include "utils/Light/Light.hpp"
#include <iostream>

namespace ES::Plugin::OpenGL::System {
const int DEFAULT_WIDTH = 800;
const int DEFAULT_HEIGHT = 800;

void UpdateKey(ES::Engine::Core &core);

void UpdateButton(ES::Engine::Core &core);

void SaveLastMousePos(ES::Engine::Core &core);

void InitGLFW(const ES::Engine::Core &core);

void SetupGLFWHints(const ES::Engine::Core &core);

void CreateGLFWWindow(ES::Engine::Core &core);
void LinkGLFWContextToGL(ES::Engine::Core &core);

void InitGLEW(const ES::Engine::Core &core);

void CheckGLEWVersion(const ES::Engine::Core &core);

void GLFWEnableVSync(ES::Engine::Core &core);

void UpdatePosCursor(ES::Engine::Core &core);

void MouseDragging(ES::Engine::Core &core);
void SwapBuffers(ES::Engine::Core &core);
void PollEvents(ES::Engine::Core &core);

void LoadShaderManager(ES::Engine::Core &core);
void LoadDefaultShader(ES::Engine::Core &core);

void SetupShaderUniforms(ES::Engine::Core &core);
void LoadMaterialCache(ES::Engine::Core &core);

void CreateCamera(ES::Engine::Core &core);

void UpdateMatrices(ES::Engine::Core &core);

void GLClearColor(const ES::Engine::Core &core);
void GLClearDepth(const ES::Engine::Core &core);
void GLEnableDepth(const ES::Engine::Core &core);
void GLEnableCullFace(const ES::Engine::Core &core);

void SetupLights(ES::Engine::Core &core);
void SetupCamera(ES::Engine::Core &core);

void RenderMeshes(ES::Engine::Core &core);
} // namespace ES::Plugin::OpenGL::System
