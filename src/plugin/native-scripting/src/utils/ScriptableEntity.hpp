#pragma once

#include "Entity.hpp"

namespace ES::Plugin::NativeScripting::Resource {
class ScriptableEntity {
  public:
    template <typename T> T &GetComponent(ES::Engine::Registry &registry) { return entity.GetComponents<T>(registry); }

    ES::Engine::Entity entity;
};
} // namespace ES::Plugin::NativeScripting::Resource
