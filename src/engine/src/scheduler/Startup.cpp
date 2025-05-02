#include "Startup.hpp"

void ES::Engine::Scheduler::Startup::RunSystems()
{
    for (auto const &system : this->_systemsList.GetSystems())
    {
        if (system->enabled)
        {
            (*system)(_core);
        }
    }

    _callback();
}
