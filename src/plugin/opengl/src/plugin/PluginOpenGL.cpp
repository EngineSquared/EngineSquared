#include "PluginOpenGL.hpp"
#include "Startup.hpp"
#include "system/AllSystems.hpp"
#include "system/WindowSystem.hpp"

void ES::Plugin::OpenGL::Plugin::Bind()
{

    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::InitGLFW);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::SetupGLFWHints);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::CreateWindowSystem);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::LinkGLFWContextToGL);

    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::InitGLEW);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::CheckGLEWVersion);

    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::EnableVSync);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::Window::System::LoadButtons);

    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadFontManager);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadMaterialCache);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadShaderManager);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadDefaultShader);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadDefaultTextShader);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::CreateCamera);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::SetupShaderUniforms);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::SetupTextShaderUniforms);
    RegisterSystems<ES::Engine::Scheduler::Startup>(ES::Plugin::OpenGL::System::LoadGLBufferManager);

    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::UpdateKey);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::UpdatePosCursor);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::UpdateButton);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::SaveLastMousePos);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::SwapBuffers);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::PollEvents);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::Window::System::StopSystems);

    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::MouseDragging);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::UpdateMatrices);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLClearColor);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLClearDepth);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLEnableDepth);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::GLEnableCullFace);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::SetupCamera);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::SetupLights);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::LoadGLBuffer);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::RenderMeshes);
    RegisterSystems<ES::Engine::Scheduler::Update>(ES::Plugin::OpenGL::System::RenderText);
}
