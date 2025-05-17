#include "Init.hpp"
#include "Engine.hpp"
#include <entt/entt.hpp>

void ES::Plugin::RenderingPipeline::Init::RunSystems()
{
    for (auto const &system : this->GetSystems())
    {
        RunSystem(system.get(), _core);
    }

    _core.DeleteScheduler<ES::Plugin::RenderingPipeline::Init>();
}
