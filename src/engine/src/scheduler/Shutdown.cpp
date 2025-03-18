#include "Shutdown.hpp"

void ES::Engine::Scheduler::Shutdown::RunSystems()
{
    for (auto const &system : this->_systemsList.GetSystems())
    {
        (*system)(_registry);
    }

    _callback();
}
