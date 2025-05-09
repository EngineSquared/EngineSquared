#include "AScheduler.hpp"

namespace ES::Engine::Scheduler {
void AScheduler::Disable(ES::Utils::FunctionContainer::FunctionID &id)
{
    if (_enabledSystemsList.Contains(id))
    {
        id = _disabledSystemsList.AddFunction(_enabledSystemsList.DeleteFunction(id));
    }
    else if (_disabledSystemsList.Contains(id))
    {
        ES::Utils::Log::Warn(fmt::format("System with id {} is already disabled", id));
    }
    else
    {
        ES::Utils::Log::Warn(fmt::format("System with id {} don't exist in the scheduler", id));
    }
}

void AScheduler::Enable(ES::Utils::FunctionContainer::FunctionID &id)
{
    if (_disabledSystemsList.Contains(id))
    {
        id = _enabledSystemsList.AddFunction(_disabledSystemsList.DeleteFunction(id));
    }
    else if (_enabledSystemsList.Contains(id))
    {
        ES::Utils::Log::Warn(fmt::format("System with id {} is already enabled", id));
    }
    else
    {
        ES::Utils::Log::Warn(fmt::format("System with id {} don't exist in the scheduler", id));
    }
}
} // namespace ES::Engine::Scheduler
