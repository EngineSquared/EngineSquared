#pragma once

#include "entity/Entity.hpp"

namespace Plugin::NativeScripting::Utils {
class ScriptableEntity {
  public:
    template <typename T> T &GetComponent(Engine::Core &core) { return entity.GetComponents<T>(core); }

    Engine::Entity entity;
};
} // namespace Plugin::NativeScripting::Utils
