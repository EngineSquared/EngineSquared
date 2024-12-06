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
    explicit Startup(Registry &registry, std::function<void()> callback) : IScheduler(registry), _callback(callback) {}
    void RunSystems(std::vector<USystem> systems) override;

  private:
    std::function<void()> _callback;
};
} // namespace ES::Engine::Scheduler
