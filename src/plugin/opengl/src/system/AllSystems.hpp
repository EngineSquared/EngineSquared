#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Entity.hpp"
#include "GLFWWindow.hpp"
#include "MaterialCache.hpp"
#include "Model.hpp"
#include "Object.hpp"
#include "ShaderManager.hpp"
#include "core/Core.hpp"
#include "entt/entt.hpp"
#include "resource/Buttons/Buttons.hpp"
#include "resource/Camera/Camera.hpp"

namespace ES::Plugin::OpenGL::System {
const int DEFAULT_WIDTH = 800;
const int DEFAULT_HEIGHT = 800;

struct Light {
    glm::vec4 Position;
    glm::vec3 Intensity;
};

void UpdateKey(ES::Engine::Core &core);

void UpdateButton(ES::Engine::Core &core);

void SaveLastMousePos(ES::Engine::Core &core);

void InitGLFW(const ES::Engine::Core &core);

void SetupGLFWHints(const ES::Engine::Core &core);

void CreateGLFWWindow(ES::Engine::Core &core);
void LinkGLFWContextToGL(ES::Engine::Core &core);

void InitGLEW(ES::Engine::Core &core);

void CheckGLEWVersion(const ES::Engine::Core &core);

void GLFWEnableVSync(ES::Engine::Core &core);

void UpdatePosCursor(ES::Engine::Core &core);

void MouseDragging(ES::Engine::Core &core);
void SwapBuffers(ES::Engine::Core &core);
void PollEvents(ES::Engine::Core &core);

void LoadShaderManager(ES::Engine::Core &core);

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
