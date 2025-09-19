#pragma once

#include <entt/entt.hpp>

#include "scheduler/AScheduler.hpp"

namespace Engine::Scheduler {
/**
 * @brief Shutdown scheduler that runs systems only once when engine shuts down
 */
class Shutdown : public AScheduler {
  public:
    explicit Shutdown(Core &core) : AScheduler(core) {}
    void RunSystems() override;
};
} // namespace Engine::Scheduler
