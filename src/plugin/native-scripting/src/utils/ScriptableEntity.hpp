#pragma once

#include "entity/Entity.hpp"

namespace NativeScripting::Utils {
class ScriptableEntity {
  public:
    explicit ScriptableEntity(Engine::Entity ent) : entity(ent) {}
    virtual ~ScriptableEntity() = default;
    template <typename T> T &GetComponent(Engine::Core &core) { return entity.GetComponents<T>(core); }

    Engine::Entity entity;
};
} // namespace NativeScripting::Utils
