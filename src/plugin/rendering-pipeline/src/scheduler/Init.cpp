#include "scheduler/Init.hpp"
#include "Engine.hpp"
#include <entt/entt.hpp>

void RenderingPipeline::Init::RunSystems()
{
    for (auto const &system : this->GetSystems())
    {
        RunSystem(system.get(), _core);
    }

    _core.DeleteScheduler<RenderingPipeline::Init>();
}
