#include "Startup.hpp"

void ES::Engine::Scheduler::Startup::RunSystems()
{
    for (auto const &system : this->_systemsList.GetSystems())
    {
        (*system)(_registry);
    }

    _callback();
}
