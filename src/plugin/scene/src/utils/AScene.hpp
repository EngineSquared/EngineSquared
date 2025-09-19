#pragma once

#include <string>

#include "Engine.hpp"

namespace Plugin::Scene::Utils {
class AScene {
  public:
    AScene(void) = default;
    virtual ~AScene() = default;

    /**
     * @brief Method used to instantiate new entities with their components
     *
     * @param core   The core where the entities will be created
     */
    virtual void Load(Engine::Core &core) final { _onCreate(core); }

    /**
     * @brief Method used to destroy all entities and their components
     *
     * @param core The core where the entities will be destroyed
     */
    virtual void Unload(Engine::Core &core) final { _onDestroy(core); }

  protected:
    virtual void _onCreate(Engine::Core &core) = 0;

    virtual void _onDestroy(Engine::Core &core) = 0;
};
} // namespace Plugin::Scene::Utils
