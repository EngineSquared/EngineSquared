#include "scheduler/Setup.hpp"
#include "Engine.hpp"
#include <entt/entt.hpp>

void ES::Plugin::RenderingPipeline::Setup::RunSystems()
{
    for (auto const &system : this->GetSystems())
    {
        RunSystem(system.get(), _core);
    }

    _core.DeleteScheduler<ES::Plugin::RenderingPipeline::Setup>();
}
