#include "scheduler/Init.hpp"
#include "Engine.hpp"
#include <entt/entt.hpp>

void Plugin::RenderingPipeline::Init::RunSystems()
{
    for (auto const &system : this->GetSystems())
    {
        RunSystem(system.get(), _core);
    }

    _core.DeleteScheduler<Plugin::RenderingPipeline::Init>();
}
