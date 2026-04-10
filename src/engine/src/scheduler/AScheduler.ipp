#include "scheduler/AScheduler.hpp"

namespace Engine::Scheduler {
template <typename... TSystems>
decltype(auto) AScheduler::AddSystems(TSystems... systems)
{
    return _enabledSystemsList.AddSystems(systems...);
}
} // namespace Engine::Scheduler
