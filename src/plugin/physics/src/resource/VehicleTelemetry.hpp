/**************************************************************************
 * EngineSquared v0.2.0
 * @file VehicleTelemetry.hpp
 * @brief Resource exposing per-vehicle RPM telemetry
 **************************************************************************/

#pragma once

#include <functional>
#include <optional>
#include <unordered_map>

#include "Engine.hpp"

namespace Physics::Resource {

class VehicleTelemetry {
  public:
    void SetRPM(Engine::EntityId entity, float rpm) { _rpmByEntity[entity] = rpm; }
    std::optional<float> GetRPM(Engine::EntityId entity) const
    {
        auto it = _rpmByEntity.find(entity);
        if (it == _rpmByEntity.end())
            return std::nullopt;
        return it->second;
    }
    void Clear(Engine::EntityId entity) { _rpmByEntity.erase(entity); }

  private:
    struct EntityIdHash {
        size_t operator()(const Engine::EntityId &id) const noexcept
        {
            using VT = Engine::EntityId::ValueType;
            return std::hash<VT>{}(static_cast<VT>(id));
        }
    };
    struct EntityIdEqual {
        bool operator()(const Engine::EntityId &a, const Engine::EntityId &b) const noexcept
        {
            return static_cast<Engine::EntityId::ValueType>(a) == static_cast<Engine::EntityId::ValueType>(b);
        }
    };
    std::unordered_map<Engine::EntityId, float, EntityIdHash, EntityIdEqual> _rpmByEntity;
};

} // namespace Physics::Resource
