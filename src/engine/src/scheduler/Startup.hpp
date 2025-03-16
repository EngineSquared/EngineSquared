#pragma once

#include <entt/entt.hpp>

#include "AScheduler.hpp"

namespace ES::Engine::Scheduler {
/**
 * @brief Startup scheduler that runs systems only once
 */
class Startup : public AScheduler {
  public:
    explicit Startup(Core &registry, const std::function<void()> &callback) : AScheduler(registry), _callback(callback)
    {
    }
    void RunSystems(USystemList &systems) override;

  private:
    std::function<void()> _callback;
};
} // namespace ES::Engine::Scheduler
