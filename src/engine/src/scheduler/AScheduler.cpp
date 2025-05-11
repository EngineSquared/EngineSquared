#include "AScheduler.hpp"

#include "Logger.hpp"

namespace ES::Engine::Scheduler {
void AScheduler::Disable(ES::Utils::FunctionContainer::FunctionID id)
{
    if (_enabledSystemsList.Contains(id))
    {
        _disabledSystemsList.AddFunction(_enabledSystemsList.DeleteFunction(id));
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

void AScheduler::Enable(ES::Utils::FunctionContainer::FunctionID id)
{
    if (_disabledSystemsList.Contains(id))
    {
        _enabledSystemsList.AddFunction(_disabledSystemsList.DeleteFunction(id));
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

void AScheduler::RunSystem(const SystemBase *system, Core &core)
{
    if (!_shouldRunSystems)
    {
        return;
    }

    try
    {
        (*system)(core);
    }
    catch (const std::exception &e) // NOSONAR
    {
        if (_errorPolicy != SchedulerErrorPolicy::Silent)
        {
            ES::Utils::Log::Error(fmt::format("System {} failed: {}", system->GetID(), e.what()));
        }
        else
        {
            ES::Utils::Log::Debug(fmt::format("System {} failed: {}", system->GetID(), e.what()));
        }

        switch (_errorPolicy)
        {
        case SchedulerErrorPolicy::LogAndStop:
            _shouldRunSystems = false;
            _shouldRunNextScheduler = false;
            break;
        case SchedulerErrorPolicy::LogAndFinishScheduler:
            _shouldRunSystems = true;
            _shouldRunNextScheduler = false;
            break;
        default:
            break;
        }
    }
}
} // namespace ES::Engine::Scheduler
