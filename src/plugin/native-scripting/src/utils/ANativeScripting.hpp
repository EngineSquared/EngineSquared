#pragma once

#include <string>

#include "Engine.hpp"

namespace ES::Plugin::NativeScripting::Utils {
class ANativeScripting {
  public:
    ANativeScripting(void) = default;
    virtual ~ANativeScripting() = default;

    /**
     * @brief Method used to instantiate new entities with their components
     *
     * @param reg   The registry where the entities will be created
     */
    virtual void Load(ES::Engine::Core &reg) final { _onCreate(reg); }

    /**
     * @brief Method used to destroy all entities and their components
     *
     * @param registry The registry where the entities will be destroyed
     */
    virtual void Unload(ES::Engine::Core &registry) final { _onDestroy(registry); }

  protected:
    virtual void _onCreate(ES::Engine::Core &registry) = 0;

    virtual void _onDestroy(ES::Engine::Core &registry) = 0;
};
} // namespace ES::Plugin::NativeScripting::Utils
