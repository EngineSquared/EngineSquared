#include "system/commandCreation/ExecuteRenderPass.hpp"
#include "resource/RenderGraphContainer.hpp"

void Graphic::System::ExecuteRenderPass(Engine::Core &core)
{
    auto &renderPassContainer = core.GetResource<Graphic::Resource::RenderGraphContainer>();

    if (renderPassContainer.HasDefault())
        renderPassContainer.GetDefault().Execute(core);
}
