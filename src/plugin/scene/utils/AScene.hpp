#pragma once

#include <string>

#include "Engine.hpp"

namespace ES::Plugin::Scene::Utils {
class AScene {
  public:
    AScene(void){};
    ~AScene() = default;

    virtual void Load(ES::Engine::Registry &reg) final { OnCreate(reg); }

    virtual void Unload(ES::Engine::Registry &registry) final { OnDestroy(registry); }

  protected:
    virtual void OnCreate(ES::Engine::Registry &registry) = 0;

    virtual void OnDestroy(ES::Engine::Registry &registry){};
};
} // namespace ES::Plugin::Scene::Utils
