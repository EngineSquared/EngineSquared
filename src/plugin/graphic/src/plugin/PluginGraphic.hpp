#pragma once

#include "plugin/APlugin.hpp"

namespace Plugin::Graphic {
class Plugin : public Engine::APlugin {
  public:
    explicit Plugin(Engine::Core &core)
        : Engine::APlugin(core){
            // empty
          };
    ~Plugin() = default;

    void Bind() final;
};
} // namespace Plugin::Graphic
