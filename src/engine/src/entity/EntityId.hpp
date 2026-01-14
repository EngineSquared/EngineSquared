#pragma once

#include "Id.hpp"
#include <fmt/format.h>

namespace Engine {

struct EntityId : Id {
    using ValueType = Id::ValueType;
    constexpr explicit(false) EntityId(ValueType v = NullValue()) : Id{v} {}
    constexpr operator ValueType() const { return value; }
    static constexpr EntityId Null() { return EntityId{NullValue()}; }
};

static_assert(sizeof(EntityId) == sizeof(Id), "EntityId size must be equal to Id size");
} // namespace Engine

namespace entt {
template <> struct entt_traits<Engine::EntityId> : entt_traits<entt::id_type> {
    using value_type = Engine::EntityId;
};
} // namespace entt

template <> struct fmt::formatter<Engine::EntityId> : fmt::formatter<std::string> {
    template <typename FormatContext> auto format(const Engine::EntityId &entityId, FormatContext &ctx) const
    {
        if (entityId.IsNull()) {
            return fmt::formatter<std::string>::format("null_entity", ctx);
        }
        return fmt::formatter<std::string>::format(Log::EntityToDebugString(entityId.value), ctx);
    }
};
