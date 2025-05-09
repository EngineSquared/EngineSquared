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
    void Disable(ES::Utils::FunctionContainer::FunctionID &id);

    /**
     * @brief Enable a system. It will add the system to the "main" system list, and it will be returned by the
     * GetSystems function.
     *
     * @param id The system to enable
     */
    void Enable(ES::Utils::FunctionContainer::FunctionID &id);

  protected:
    Core &_core;

  private:
    SystemContainer _enabledSystemsList;
    SystemContainer _disabledSystemsList;
};
} // namespace ES::Engine::Scheduler
