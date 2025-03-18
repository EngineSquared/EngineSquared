#include "Shutdown.hpp"
#include "Core.hpp"

void ES::Engine::Scheduler::Shutdown::RunSystems()
{
    if (_registry.IsRunning())
    {
        return;
    }
    for (auto const &system : this->_systemsList.GetSystems())
    {
        (*system)(_registry);
    }
}
