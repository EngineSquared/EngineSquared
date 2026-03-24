#pragma once

#include "plugin/APlugin.hpp"

namespace Graphic {
class Plugin : public Engine::APlugin {
  public:
    explicit Plugin(Engine::Core &core)
        : Engine::APlugin(core){
              // empty
          };
    ~Plugin() override = default;

    void Bind() final;
};
} // namespace Graphic
