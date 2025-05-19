#pragma once

#include "AScheduler.hpp"

namespace ES::Engine::Scheduler {
/**
 * @brief Startup scheduler that runs systems only once
 */
class Startup : public AScheduler {
  public:
    explicit Startup(Core &core, const std::function<void()> &callback) : AScheduler(core), _callback(callback) {}
    void RunSystems() override;

  private:
    std::function<void()> _callback;
};
} // namespace ES::Engine::Scheduler
