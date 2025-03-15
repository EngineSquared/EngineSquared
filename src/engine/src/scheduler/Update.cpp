#include "Update.hpp"

void ES::Engine::Scheduler::Update::RunSystems()
{
    for (auto const &system : this->_systemsList.GetSystems())
    {
        (*system)(_registry);
    }
}
