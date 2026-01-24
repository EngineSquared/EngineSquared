#pragma once

#include "plugin/APlugin.hpp"

namespace Rmlui {
class Plugin : public Engine::APlugin {
  public:
    using APlugin::APlugin;
    ~Plugin() override = default;

    void Bind() final;
};
} // namespace Rmlui
