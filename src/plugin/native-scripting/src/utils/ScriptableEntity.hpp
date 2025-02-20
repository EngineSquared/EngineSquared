#pragma once

#include "Entity.hpp"

namespace ES::Plugin::NativeScripting::Utils {
class ScriptableEntity {
  public:
    template <typename T> T &GetComponent(ES::Engine::Core &core) { return entity.GetComponents<T>(core); }

    ES::Engine::Entity entity;
};
} // namespace ES::Plugin::NativeScripting::Utils
