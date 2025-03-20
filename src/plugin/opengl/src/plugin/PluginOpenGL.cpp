#include "PluginOpenGL.hpp"
#include "Startup.hpp"
#include "plugin/PluginWindow.hpp"
#include "system/AllSystems.hpp"

void ES::Plugin::OpenGL::Plugin::Bind()
{
    RequirePlugins<ES::Plugin::Window::Plugin>();
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::InitGLEW);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::CheckGLEWVersion);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadMaterialCache);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadShaderManager);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadDefaultShader);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::CreateCamera);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::SetupShaderUniforms);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::UpdateMatrices);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLClearColor);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLClearDepth);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLEnableDepth);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLEnableCullFace);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::SetupCamera);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::SetupLights);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::RenderMeshes);
}
