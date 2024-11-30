#pragma once

#include <entt/entt.hpp>

#include "IScheduler.hpp"

namespace ES::Engine {
class Registry;
}

namespace ES::Engine::Scheduler {
using USystem = std::function<void(Registry &)>;

class Startup : public IScheduler {
  public:
    Startup(Registry &registry) : IScheduler(registry) {}
    void operator()(std::vector<USystem> systems) override
    {
        if (!_firstRun)
        {
            return;
        }

        for (auto &system : systems)
        {
            system(_registry);
        }

        _firstRun = false;
    }

  private:
    bool _firstRun = true;
};
} // namespace ES::Engine::Scheduler
