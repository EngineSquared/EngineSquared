#include "PluginOpenGl.hpp"
#include "Startup.hpp"
#include "resource/Buttons/Buttons.hpp"
#include "system/AllSystems.hpp"

void ES::Plugin::OpenGL::Plugin::Build()
{
    RegisterResource<ES::Plugin::OpenGL::Resource::Buttons>({});

    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::InitGLFW);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::SetupGLFWHints);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::CreateGLFWWindow);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LinkGLFWContextToGL);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::InitGLEW);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::CheckGLEWVersion);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::GLFWEnableVSync);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::SetupGLFWHints);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadMaterialCache);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadShaderManager);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::CreateCamera);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::SetupShaderUniforms);

    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::UpdateKey);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::UpdatePosCursor);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::UpdateButton);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::SaveLastMousePos);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::UpdateMatrices);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLClearColor);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLClearDepth);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLEnableDepth);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLEnableCullFace);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::SetupCamera);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::SetupLights);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::RenderMeshes);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::SwapBuffers);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::PollEvents);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::MouseDragging);
}
