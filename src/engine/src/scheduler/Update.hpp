#pragma once

#include <entt/entt.hpp>

#include "IScheduler.hpp"

namespace ES::Engine {
class Registry;
}

namespace ES::Engine::Scheduler {
using USystem = std::function<void(Registry &)>;

class Update : public IScheduler {
  public:
    Update(Registry &registry) : IScheduler(registry) {}
    void operator()(std::vector<USystem> systems)
    {
        for (auto &system : systems)
        {
            system(_registry);
        }
    }
};
} // namespace ES::Engine::Scheduler