#pragma once

#include <entt/entt.hpp>

#include "AScheduler.hpp"

namespace ES::Engine::Scheduler {
/**
 * @brief Update scheduler that runs systems every time it is called
 */
class Update : public AScheduler {
  public:
    explicit Update(Registry &registry) : AScheduler(registry) {}
    void RunSystems(std::vector<USystem> systems) override;
};
} // namespace ES::Engine::Scheduler
