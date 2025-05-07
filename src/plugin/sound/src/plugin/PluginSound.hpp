#pragma once

#include "APlugin.hpp"

namespace ES::Plugin::Sound {
class Plugin : public ES::Engine::APlugin {
  public:
    explicit Plugin(ES::Engine::Core &core)
        : ES::Engine::APlugin(core){
              // empty
          };
    ~Plugin() = default;

    void Bind() final;
};
} // namespace ES::Plugin::Sound
