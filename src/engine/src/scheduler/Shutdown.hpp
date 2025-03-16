#pragma once

#include <entt/entt.hpp>

#include "AScheduler.hpp"

namespace ES::Engine::Scheduler {
/**
 * @brief Shutdown scheduler that runs systems only once when engine shuts down
 */
class Shutdown : public AScheduler {
  public:
    explicit Shutdown(Core &registry, std::function<void()> callback) : AScheduler(registry), _callback(callback)
    {
    }
    void RunSystems(USystemList &systems) override;

  private:
    std::function<void()> _callback;
};
} // namespace ES::Engine::Scheduler
