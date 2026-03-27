#pragma once

#include <entt/entt.hpp>

#include "scheduler/AScheduler.hpp"

namespace Engine::Scheduler {
/// @class Shutdown
/// @brief Shutdown scheduler that runs systems only once when engine shuts down
class Shutdown : public AScheduler {
  public:
    /// @copydoc Engine::Scheduler::AScheduler::AScheduler
    using AScheduler::AScheduler;

    /// @copydoc Engine::Scheduler::IScheduler::RunSystems
    void RunSystems() override;
};
} // namespace Engine::Scheduler
