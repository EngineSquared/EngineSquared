#pragma once

#include "IScheduler.hpp"

#include "system/System.hpp"

#include <set>
#include <typeindex>

namespace Engine::Scheduler {
/**
 * @brief Interface to be implemented for every schedulers.
 */
class AScheduler : public IScheduler {
  public:
    explicit AScheduler(Core &core) : _core(core) {}

    inline decltype(auto) GetSystems() { return _enabledSystemsList.GetSystems(); }

    /**
     * @brief Get the list of enabled systems
     *
     * @tparam  TSystems    Type of systems to add. (can be omitted)
     * @param   systems     The systems to add
     *
     * @return  The list of enabled systems ids
     */
    template <typename... TSystems> inline decltype(auto) AddSystems(TSystems... systems)
    {
        return _enabledSystemsList.AddSystems(systems...);
    }

    /**
     * @brief Disable a system. It will remove the system from the "main" system list, and it will not be returned by
     * the GetSystems function.
     */
    void Disable(FunctionUtils::FunctionID id);

    /**
     * @brief Enable a system. It will add the system to the "main" system list, and it will be returned by the
     * GetSystems function.
     *
     * @param id The system to enable
     */
    void Enable(FunctionUtils::FunctionID id);

    /**
     * @brief Execute a system according to the scheduler policy
     *
     * @param system The system to run
     * @param core The core to pass to the system
     */
    void RunSystem(const SystemBase *system, Core &core);

    /**
     * @brief Whether the next scheduler should run or not
     *
     * @return true if the next scheduler should run, false otherwise
     */
    inline bool ShouldRunNextScheduler() const { return _shouldRunNextScheduler; }

    /**
     * @brief Get the error policy
     *
     * @return The error policy
     */
    inline SchedulerErrorPolicy GetErrorPolicy() const override { return _errorPolicy; }

    /**
     * @brief Set the error policy
     *
     * @param errorPolicy The error policy
     */
    inline void SetErrorPolicy(SchedulerErrorPolicy errorPolicy) override { _errorPolicy = errorPolicy; }

  protected:
    Core &_core;

  private:
    SystemContainer _enabledSystemsList;
    SystemContainer _disabledSystemsList;
    bool _shouldRunSystems = true;
    bool _shouldRunNextScheduler = true;
    SchedulerErrorPolicy _errorPolicy = SchedulerErrorPolicy::LogAndContinue;
};
} // namespace Engine::Scheduler
