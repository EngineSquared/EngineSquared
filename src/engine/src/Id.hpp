#pragma once

#include "EntityToIDString.hpp"
#include <entt/entt.hpp>
#include <fmt/format.h>

namespace Engine {

/// @struct BasicId
/// @brief An Id class for creating strongly-typed ID wrappers. This template provides a common interface for ID types
/// by wrapping a value type with type-safe operations. Derived classes should implement static NullValue() and Null()
/// methods to define null/invalid ID semantics.
/// @tparam TDerived The derived class type (CRTP pattern)
/// @tparam TValue The underlying value type for the ID (e.g., int, uint64_t, etc.)
/// @note Derived classes must provide:
///       - static constexpr TValue NullValue() - Returns the value representing a null/invalid ID
///       - static constexpr TDerived Null() - Returns a null instance of the derived type
template <typename TDerived, typename TValue> struct BasicId {
    /// @brief The underlying value of the ID.
    TValue value;

    /// @brief Constructor for BasicId. Allows implicit construction from the underlying value type, defaulting to the
    ///     null value.
    /// @param v The value to initialize the ID with. Defaults to the null value defined by the derived class.
    /// @return An instance of the derived ID type initialized with the given value.
    /// @todo rename v to something more descriptive, like idValue or rawValue
    /// @todo Put the implementation in the cpp file if possible
    constexpr explicit(false) BasicId(TValue v = TDerived::NullValue()) : value{v} {}

    /// @brief Get the null value for this ID type. This should be defined by the derived class to specify what
    ///     constitutes a null/invalid ID.
    /// @return The value representing a null/invalid ID for this type.
    /// @todo Put the implementation in the cpp file if possible
    static constexpr TDerived Null() { return TDerived::Null(); }

    /// @brief Checks if the ID is null/invalid by comparing its value to the null value defined by the derived class.
    /// @return True if the ID is null/invalid, false otherwise.
    /// @todo Put the implementation in the cpp file if possible
    constexpr bool IsNull() const { return value == TDerived::NullValue(); }
};

/// @struct Id
/// @brief A strongly-typed identifier wrapper based on entt::id_type. This structure provides a type-safe wrapper
///     around EnTT's id_type, offering implicit conversion to the underlying type for seamless integration with EnTT
///     components. It inherits from BasicId to provide null checking and other common ID operations.
/// @note Supports implicit construction from entt::id_type values
/// @note Supports implicit conversion back to entt::id_type for compatibility
/// @note Size is guaranteed to match entt::id_type through static assertion
struct Id : public BasicId<Id, entt::id_type> {
    using ValueType = entt::id_type;

    /// @brief Constructor for Id.
    /// @param v The value to initialize the ID with. Defaults to the null value defined by the NullValue() method.
    /// @todo Put the implementation in the cpp file if possible
    /// @todo rename v to something more descriptive, like idValue or rawValue
    constexpr Id(ValueType v = NullValue()) : BasicId<Id, ValueType>{v} {}

    /// @brief Implicit conversion operator to the underlying entt::id_type, allowing seamless use of Id instances where
    ///     entt::id_type is expected.
    constexpr operator ValueType() const { return value; }

    /// @brief Get the null value for this ID type. This returns the null value defined by EnTT's id_type, which is
    ///     typically
    /// @return The value representing a null/invalid ID for this type.
    /// @todo Put the implementation in the cpp file if possible
    static constexpr ValueType NullValue() { return entt::null; }

    /// @brief Get a null instance of Id, which has its value set to the null value defined by NullValue().
    /// @return A null instance of Id.
    /// @todo Put the implementation in the cpp file if possible
    static constexpr Id Null() { return Id{NullValue()}; }
};

/// @brief Static assertion to ensure that the size of Id matches the size of entt::id_type, guaranteeing that there is
///     no additional overhead from the wrapper and that it can be used interchangeably with entt::id_type without
///     penalties.
static_assert(sizeof(Id) == sizeof(Id::ValueType), "Id size must be equal to entt::id_type size");

/// @struct StringId
/// @brief A strongly-typed string identifier wrapper based on entt::hashed_string. This structure provides a type-safe
///     wrapper around EnTT's hashed_string, designed for efficient compile-time and runtime string hashing. Unlike Id,
///     this is specifically for string-based identifiers and does not provide implicit conversion to the underlying
///     type. String hashing allows for fast comparisons while maintaining human-readable identifiers during development
///     and debugging.
/// @note Supports implicit construction from entt::hashed_string values
/// @note Size is guaranteed to match entt::hashed_string through static assertion
/// @note Useful for named resources, tags, and other string-based identification schemes
struct StringId : public BasicId<StringId, entt::hashed_string> {
    /// @brief The underlying value type for StringId, which is entt::hashed_string. This type encapsulates the string
    ///     data and its hash, allowing for efficient comparisons and lookups while maintaining the original string for
    ///     debugging purposes.
    using ValueType = entt::hashed_string;

    /// @brief Constructor for StringId.
    /// @param v The value to initialize the StringId with. Defaults to the null value defined by the NullValue()
    ///     method.
    /// @todo Put the implementation in the cpp file if possible
    constexpr StringId(ValueType v = NullValue()) : BasicId<StringId, ValueType>{v} {}

    /// @brief Get the null value for this StringId type. This returns a default-constructed entt::hashed_string.
    /// @return The value representing a null/invalid StringId for this type.
    /// @todo Put the implementation in the cpp file if possible
    static constexpr ValueType NullValue() { return entt::hashed_string{}; }

    /// @brief Get a null instance of StringId, which has its value set to the null value defined by NullValue().
    /// @return A null instance of StringId.
    /// @todo Put the implementation in the cpp file if possible
    static constexpr StringId Null() { return StringId{NullValue()}; }
};
/// @brief Static assertion to ensure that the size of StringId matches the size of entt::hashed_string, guaranteeing
///     that there is no additional overhead from the wrapper and that it can be used interchangeably with
///     entt::hashed_string without penalties.
static_assert(sizeof(StringId) == sizeof(StringId::ValueType),
              "StringId size must be equal to entt::hashed_string size");
} // namespace Engine

/// @brief Specialization of entt::entt_traits for Engine::Id, allowing it to be used as an entity identifier type in
///     EnTT's ECS system. This specialization defines the necessary traits for Engine::Id to be recognized as a valid
///     identifier type by EnTT, enabling its use in components, systems, and other EnTT features that rely on entity
///     identifiers.
template <> struct entt::entt_traits<Engine::Id> : entt::entt_traits<entt::id_type> {
    using value_type = Engine::Id;
};

/// @brief Specialization of fmt::formatter for Engine::Id, allowing it to be formatted using the fmt library.
template <> struct fmt::formatter<Engine::Id> : fmt::formatter<entt::id_type> {
    /// @brief Format an Engine::Id using the underlying entt::id_type formatter.
    /// @tparam FormatContext The type of the format context provided by fmt during formatting.
    /// @param id The Engine::Id instance to format.
    /// @param ctx The format context provided by fmt.
    /// @return The result of formatting the underlying entt::id_type value of the Engine::Id instance using the
    ///     appropriate formatter for entt::id_type.
    /// @todo Put the implementation in the inl file if possible
    template <typename FormatContext> auto format(const Engine::Id &id, FormatContext &ctx) const
    {
        return fmt::formatter<entt::id_type>::format(id.value, ctx);
    }
};

/// @brief Specialization of fmt::formatter for Engine::StringId, allowing it to be formatted using the fmt library.
///     This formatter formats the StringId by extracting the original string representation from the underlying
///     entt::hashed_string and formatting it as a std::string_view, which provides a human-readable representation of
///     the StringId for debugging and logging purposes.
template <> struct fmt::formatter<Engine::StringId> : fmt::formatter<std::string_view> {
    /// @brief Format an Engine::StringId by extracting its original string representation and formatting it as a
    ///     std::string_view.
    /// @tparam FormatContext The type of the format context provided by fmt during formatting.
    /// @param id The Engine::StringId instance to format.
    /// @param ctx The format context provided by fmt.
    /// @return The result of formatting the original string representation of the Engine::StringId instance, extracted
    ///     from its underlying entt::hashed_string value, as a std::string_view for human-readable output.
    /// @todo Put the implementation in the inl file if possible
    template <typename FormatContext> auto format(const Engine::StringId &id, FormatContext &ctx) const
    {
        return fmt::formatter<std::string_view>::format(std::string_view(id.value.data(), id.value.size()), ctx);
    }
};
