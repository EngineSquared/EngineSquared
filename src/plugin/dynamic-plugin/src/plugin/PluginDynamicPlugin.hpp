#pragma once

#include "plugin/APlugin.hpp"

namespace DynamicPlugin {
class Plugin : public Engine::APlugin {
  public:
    explicit Plugin(Engine::Core &core)
        : Engine::APlugin(core) {
              // empty
          };
    ~Plugin() = default;

    void Attach() final;
};
} // namespace DynamicPlugin
