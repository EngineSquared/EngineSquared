#pragma once

#include "EntityToIDString.hpp"
#include <entt/entt.hpp>
#include <fmt/format.h>

namespace Engine {

/**
 * @brief A CRTP (Curiously Recurring Template Pattern) base class for creating strongly-typed ID wrappers.
 *
 * This template provides a common interface for ID types by wrapping a value type with type-safe operations.
 * Derived classes should implement static NullValue() and Null() methods to define null/invalid ID semantics.
 *
 * @tparam TDerived The derived class type (CRTP pattern)
 * @tparam TValue The underlying value type for the ID (e.g., int, uint64_t, etc.)
 *
 * @note The constructor is explicitly(false), allowing implicit conversions from TValue to BasicId.
 * @note Derived classes must provide:
 *       - static constexpr TValue NullValue() - Returns the value representing a null/invalid ID
 *       - static constexpr TDerived Null() - Returns a null instance of the derived type
 */
template <typename TDerived, typename TValue> struct BasicId {
    TValue value;

    constexpr explicit(false) BasicId(TValue v = TDerived::NullValue()) : value{v} {}

    static constexpr BasicId Null() { return TDerived::Null(); }
    constexpr bool IsNull() const { return value == TDerived::NullValue(); }
};

/**
 * @brief A strongly-typed identifier wrapper based on entt::id_type.
 *
 * This structure provides a type-safe wrapper around EnTT's id_type, offering implicit conversion
 * to the underlying type for seamless integration with EnTT components. It inherits from BasicId
 * to provide null checking and other common ID operations.
 *
 * @note Supports implicit construction from entt::id_type values
 * @note Supports implicit conversion back to entt::id_type for compatibility
 * @note Size is guaranteed to match entt::id_type through static assertion
 *
 * @see BasicId for base functionality
 * @see StringId for string-based identifiers
 */
struct Id : public BasicId<Id, entt::id_type> {
    using ValueType = entt::id_type;
    constexpr explicit(false) Id(ValueType v = NullValue()) : BasicId<Id, ValueType>{v} {}
    constexpr operator ValueType() const { return value; }
    static constexpr ValueType NullValue() { return entt::null; }
};

static_assert(sizeof(Id) == sizeof(Id::ValueType), "Id size must be equal to entt::id_type size");

/**
 * @brief A strongly-typed string identifier wrapper based on entt::hashed_string.
 *
 * This structure provides a type-safe wrapper around EnTT's hashed_string, designed for efficient
 * compile-time and runtime string hashing. Unlike Id, this is specifically for string-based
 * identifiers and does not provide implicit conversion to the underlying type.
 *
 * String hashing allows for fast comparisons while maintaining human-readable identifiers during
 * development and debugging.
 *
 * @note Supports implicit construction from entt::hashed_string values
 * @note Size is guaranteed to match entt::hashed_string through static assertion
 * @note Useful for named resources, tags, and other string-based identification schemes
 *
 * @see BasicId for base functionality
 * @see Id for numeric identifiers
 */
struct StringId : public BasicId<StringId, entt::hashed_string> {
    using ValueType = entt::hashed_string;
    constexpr explicit(false) StringId(ValueType v = NullValue()) : BasicId<StringId, ValueType>{v} {}
    static constexpr ValueType NullValue() { return entt::hashed_string{}; }
};

static_assert(sizeof(StringId) == sizeof(StringId::ValueType),
              "StringId size must be equal to entt::hashed_string size");
} // namespace Engine

namespace entt {
template <> struct entt_traits<Engine::Id> : entt_traits<entt::id_type> {
    using value_type = Engine::Id;
};
} // namespace entt

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
