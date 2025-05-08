#pragma once

#include "IScheduler.hpp"
#include <set>
#include <typeindex>

namespace ES::Engine::Scheduler {
/**
 * @brief Interface to be implemented for every schedulers
 */
class AScheduler : public IScheduler {
  public:
    explicit AScheduler(Core &core) : _core(core) {}

    inline decltype(auto) GetSystems()
    {
        return _enabledSystemsList.GetSystems();
    }

    template <typename... Systems> decltype(auto) AddSystems(Systems... systems)
    {
        return _enabledSystemsList.AddSystems(systems...);
    }

    inline void Disable(ES::Utils::FunctionContainer::FunctionID &id) {
        if (_enabledSystemsList.Contains(id)) {
            id = _disabledSystemsList.AddFunction(_enabledSystemsList.DeleteFunction(id));
        } else if (_disabledSystemsList.Contains(id)) {
            ES::Utils::Log::Warn(fmt::format("System with id {} is already disabled", id));
        } else {
            ES::Utils::Log::Warn(fmt::format("System with id {} don't exist in the scheduler", id));
        }
    }

    inline void Enable(ES::Utils::FunctionContainer::FunctionID &id) {
        if (_disabledSystemsList.Contains(id)) {
            id = _enabledSystemsList.AddFunction(_disabledSystemsList.DeleteFunction(id));
        } else if (_enabledSystemsList.Contains(id)) {
            ES::Utils::Log::Warn(fmt::format("System with id {} is already enabled", id));
        } else {
            ES::Utils::Log::Warn(fmt::format("System with id {} don't exist in the scheduler", id));
        }
    }

  protected:
    Core &_core;

  private:
    SystemContainer _enabledSystemsList;
    SystemContainer _disabledSystemsList;
};
} // namespace ES::Engine::Scheduler
