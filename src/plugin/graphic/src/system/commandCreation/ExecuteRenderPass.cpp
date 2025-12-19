#include "system/commandCreation/ExecuteRenderPass.hpp"
#include "resource/RenderGraph.hpp"

void Graphic::System::ExecuteRenderPass(Engine::Core &core)
{
    auto &renderPassManager = core.GetResource<Graphic::Resource::RenderGraph>();
    renderPassManager.Execute(core);
}
