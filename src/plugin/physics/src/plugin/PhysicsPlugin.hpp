#pragma once

#include "plugin/APlugin.hpp"

namespace Physics {
class Plugin : public Engine::APlugin {
  public:
    explicit Plugin(Engine::Core &core)
        : Engine::APlugin(core){
              // empty
          };
    ~Plugin() final = default;

    void Bind() final;
};
} // namespace Physics
