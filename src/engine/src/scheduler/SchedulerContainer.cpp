#include "SchedulerContainer.hpp"

void ES::Engine::SchedulerContainer::DeleteScheduler(std::type_index id)
{
    if (this->_schedulers.contains(id))
    {
#ifdef ES_DEBUG
        ES::Utils::Log::Info(fmt::format("Deleting scheduler: {}", id.name()));
#endif
        this->_schedulers.erase(id);
        this->_dirty = true;
        if (this->_dependencies.contains(id))
        {
            this->_dependencies.erase(id);
        }
    }
    else
    {
        ES::Utils::Log::Warn(fmt::format("Scheduler does not exist: {}", id.name()));
    }
}
