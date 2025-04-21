#pragma once

#include "APlugin.hpp"

namespace ES::Plugin::Window {
class Plugin : public ES::Engine::APlugin {
  public:
    using APlugin::APlugin;
    ;
    ~Plugin() = default;

    void Bind() final;
};
} // namespace ES::Plugin::Window
