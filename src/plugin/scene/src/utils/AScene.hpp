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
     * @param core   The core where the entities will be created
     */
    virtual void Load(ES::Engine::Core &core) final { _onCreate(core); }

    /**
     * @brief Method used to destroy all entities and their components
     *
     * @param core The core where the entities will be destroyed
     */
    virtual void Unload(ES::Engine::Core &core) final { _onDestroy(core); }

  protected:
    virtual void _onCreate(ES::Engine::Core &core) = 0;

    virtual void _onDestroy(ES::Engine::Core &core) = 0;
};
} // namespace ES::Plugin::Scene::Utils
