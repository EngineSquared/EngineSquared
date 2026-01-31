#include "Engine.pch.hpp"

#include "scheduler/AScheduler.hpp"

#include "Logger.hpp"

namespace Engine::Scheduler {
void AScheduler::Disable(FunctionUtils::FunctionID id)
{
    if (_enabledSystemsList.Contains(id))
    {
        _disabledSystemsList.AddFunction(_enabledSystemsList.DeleteFunction(id));
    }
    else if (_disabledSystemsList.Contains(id))
    {
        Log::Warn(fmt::format("System with id {} is already disabled", id));
    }
    else
    {
        Log::Warn(fmt::format("System with id {} don't exist in the scheduler", id));
    }
}

void AScheduler::Enable(FunctionUtils::FunctionID id)
{
    if (_disabledSystemsList.Contains(id))
    {
        _enabledSystemsList.AddFunction(_disabledSystemsList.DeleteFunction(id));
    }
    else if (_enabledSystemsList.Contains(id))
    {
        Log::Warn(fmt::format("System with id {} is already enabled", id));
    }
    else
    {
        Log::Warn(fmt::format("System with id {} don't exist in the scheduler", id));
    }
}

void AScheduler::RunSystem(const SystemBase *system, Core &core)
{
    if (!_shouldRunSystems)
    {
        return;
    }

    if (_errorPolicy == SchedulerErrorPolicy::Nothing)
    {
        (*system)(core);
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
            Log::Error(fmt::format("System {} failed: {}", system->GetID(), e.what()));
        }
        else
        {
            Log::Debug(fmt::format("System {} failed: {}", system->GetID(), e.what()));
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
        default: break;
        }
    }
}

void AScheduler::Remove(FunctionUtils::FunctionID id)
{
    if (_enabledSystemsList.Contains(id))
    {
        _enabledSystemsList.DeleteFunction(id);
    }
    else if (_disabledSystemsList.Contains(id))
    {
        _disabledSystemsList.DeleteFunction(id);
    }
    else
    {
        Log::Warn(fmt::format("System with id {} don't exist in the scheduler", id));
    }
}
} // namespace Engine::Scheduler
