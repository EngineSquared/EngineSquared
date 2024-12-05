#pragma once

#include <entt/entt.hpp>

#include "IScheduler.hpp"

namespace ES::Engine {
class Registry;
}

namespace ES::Engine::Scheduler {
using USystem = std::function<void(Registry &)>;

/**
 * @brief Startup scheduler that runs systems only once
 */
class Startup : public IScheduler {
  public:
    explicit Startup(Registry &registry) : IScheduler(registry) {}
    void operator()(std::vector<USystem> systems) override;

  private:
    bool _firstRun = true;
};
} // namespace ES::Engine::Scheduler
