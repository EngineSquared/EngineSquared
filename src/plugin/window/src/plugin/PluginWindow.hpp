#pragma once

#include "plugin/APlugin.hpp"

namespace Window {
class Plugin : public Engine::APlugin {
  public:
    using APlugin::APlugin;
    ;
    ~Plugin() = default;

    void Bind() final;
};
} // namespace Window
