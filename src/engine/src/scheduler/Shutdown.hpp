#pragma once

#include <entt/entt.hpp>

#include "AScheduler.hpp"

namespace ES::Engine::Scheduler {
/**
 * @brief Shutdown scheduler that runs systems only once when engine shuts down
 */
class Shutdown : public AScheduler {
  public:
    explicit Shutdown(Core &registry) : AScheduler(registry) {}
    void RunSystems() override;
};
} // namespace ES::Engine::Scheduler
