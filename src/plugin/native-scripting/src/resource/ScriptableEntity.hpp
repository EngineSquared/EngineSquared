#pragma once

#include "Entity.hpp"

namespace ES::Plugin::NativeScripting::Resource {
class ScriptableEntity {
  public:
    template <typename T> T &GetComponent() { return entity.GetComponents<T>(); }

    ES::Engine::Entity entity;

  private:
};
} // namespace ES::Plugin::NativeScripting::Resource
