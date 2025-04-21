#include "Setup.hpp"
#include "Engine.hpp"

void ES::Plugin::RenderingPipeline::Setup::RunSystems()
{
    for (auto const &system : this->_systemsList.GetSystems())
    {
        (*system)(_core);
    }

    _core.DeleteScheduler<ES::Plugin::RenderingPipeline::Setup>();
}
