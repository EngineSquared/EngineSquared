#pragma once

#include <string>

#include "Engine.hpp"

namespace ES::Plugin::Scene::Utils {
class AScene {
  public:
    AScene(void) = default;
    virtual ~AScene() = default;

    /**
     * @brief Method used to instantiate new entities with their components
     *
     * @param reg   The registry where the entities will be created
     */
    virtual void Load(ES::Engine::Registry &reg) final { _onCreate(reg); }

    /**
     * @brief Method used to destroy all entities and their components
     *
     * @param registry The registry where the entities will be destroyed
     */
    virtual void Unload(ES::Engine::Registry &registry) final { _onDestroy(registry); }

  protected:
    virtual void _onCreate(ES::Engine::Registry &registry) = 0;

    virtual void _onDestroy(ES::Engine::Registry &registry) = 0;
};
} // namespace ES::Plugin::Scene::Utils
