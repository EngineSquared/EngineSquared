#include "system/commandCreation/ExecuteDefaultRenderPass.hpp"
#include "resource/RenderPassManager.hpp"

void Graphic::System::ExecuteDefaultRenderPass(Engine::Core &core)
{
    auto &renderPassManager = core.GetResource<Graphic::Resource::RenderPassManager>();
    renderPassManager.Execute(core);
}
