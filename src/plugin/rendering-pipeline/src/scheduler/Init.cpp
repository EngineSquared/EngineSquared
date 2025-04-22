#include "Init.hpp"
#include "Engine.hpp"

void ES::Plugin::RenderingPipeline::Init::RunSystems()
{
    for (auto const &system : this->_systemsList.GetSystems())
    {
        (*system)(_core);
    }

    _core.DeleteScheduler<ES::Plugin::RenderingPipeline::Init>();
}