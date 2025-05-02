#include "Shutdown.hpp"
#include "Core.hpp"

void ES::Engine::Scheduler::Shutdown::RunSystems()
{
    if (_core.IsRunning())
    {
        return;
    }
    for (auto const &system : this->_systemsList.GetSystems())
    {
        if (system->enabled)
        {
            (*system)(_core);
        }
    }
}
