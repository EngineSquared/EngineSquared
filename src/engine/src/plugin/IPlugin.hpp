#pragma once

namespace ES::Engine {
class IPlugin {
  public:
    virtual void Bind(void) = 0;
};
} // namespace ES::Engine
