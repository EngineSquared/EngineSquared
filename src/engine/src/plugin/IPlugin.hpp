#pragma once

namespace ES::Engine {
class IPlugin {
  public:
    virtual ~IPlugin() = default;
    virtual void Bind(void) = 0;
};
} // namespace ES::Engine
