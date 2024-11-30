#pragma once

#include <entt/entt.hpp>

namespace ES::Engine {
class Registry;
}

namespace ES::Engine::Scheduler {
using USystem = std::function<void(Registry &)>;

class IScheduler {
  public:
    explicit IScheduler(Registry &registry) : _registry(registry) {}
    virtual void operator()(std::vector<USystem> systems) = 0;

  protected:
    Registry &_registry;
};
} // namespace ES::Engine::Scheduler
