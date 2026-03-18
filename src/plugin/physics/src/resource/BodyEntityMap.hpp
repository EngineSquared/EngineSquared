#pragma once

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include "entity/EntityId.hpp"
#include <Jolt/Physics/Body/BodyID.h>
#include <functional>
#include <unordered_map>

template <> struct std::hash<Engine::EntityId> {
    std::size_t operator()(const Engine::EntityId &entityId) const noexcept
    {
        return std::hash<entt::id_type>{}(entityId.value);
    }
};

namespace Physics::Resource {

class BodyEntityMap {
  public:
    BodyEntityMap() = default;
    ~BodyEntityMap() = default;

    void Add(const Engine::EntityId &entity, const JPH::BodyID &bodyID);
    void Remove(const Engine::EntityId &entity);
    void Remove(const JPH::BodyID &bodyID);
    std::uint32_t Size() const;
    const JPH::BodyID &Get(const Engine::EntityId &entity) const;
    const Engine::EntityId &Get(const JPH::BodyID &bodyID) const;
    bool Contains(const Engine::EntityId &entity) const;
    bool Contains(const JPH::BodyID &bodyID) const;

  private:
    std::unordered_map<Engine::EntityId, const JPH::BodyID *> _entityToBody;
    std::unordered_map<JPH::BodyID, const Engine::EntityId *> _bodyToEntity;
};

} // namespace Physics::Resource
