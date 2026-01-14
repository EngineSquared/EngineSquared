#pragma once

#include "EntityToIDString.hpp"
#include <entt/entt.hpp>
#include <fmt/format.h>

namespace Engine {

template <typename TDerived, typename TValue> struct BasicId {
    TValue value;

    constexpr explicit(false) BasicId(TValue v = TDerived::NullValue()) : value{v} {}

    static constexpr BasicId Null() { return TDerived::Null(); }
    constexpr bool IsNull() const { return value == TDerived::NullValue(); }
};

struct Id : public BasicId<Id, entt::id_type> {
    using ValueType = entt::id_type;
    constexpr explicit(false) Id(ValueType v = NullValue()) : BasicId<Id, ValueType>{v} {}
    static constexpr ValueType NullValue() { return entt::null; }
};

static_assert(sizeof(Id) == sizeof(entt::id_type), "Id size must be equal to entt::id_type size");

struct StringId : public BasicId<StringId, entt::hashed_string> {
    using ValueType = entt::hashed_string;
    constexpr explicit(false) StringId(ValueType v = NullValue()) : BasicId<StringId, ValueType>{v} {}
    static constexpr ValueType NullValue() { return entt::hashed_string{}; }
};

static_assert(sizeof(StringId) == sizeof(entt::hashed_string),
              "StringId size must be equal to entt::hashed_string size");
} // namespace Engine

template <> struct fmt::formatter<Engine::Id> : fmt::formatter<entt::id_type> {
    template <typename FormatContext> auto format(const Engine::Id &id, FormatContext &ctx) const
    {
        return fmt::formatter<entt::id_type>::format(id.value, ctx);
    }
};

template <> struct fmt::formatter<Engine::StringId> : fmt::formatter<std::string_view> {
    template <typename FormatContext> auto format(const Engine::StringId &id, FormatContext &ctx) const
    {
        return fmt::formatter<std::string_view>::format(std::string_view(id.value.data(), id.value.size()), ctx);
    }
};
