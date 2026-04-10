#include "Id.hpp"

namespace Engine {
template <typename TDerived, typename TValue>
constexpr BasicId<TDerived, TValue>::BasicId(TValue value_) : value{value_}
{
}
template <typename TDerived, typename TValue> constexpr TDerived BasicId<TDerived, TValue>::Null(void)
{
    return TDerived::Null();
}
template <typename TDerived, typename TValue> constexpr bool BasicId<TDerived, TValue>::IsNull(void) const
{
    return value == TDerived::NullValue();
}

constexpr Id::Id(ValueType value_) : BasicId<Id, ValueType>{value_} {}

constexpr Id::operator Id::ValueType() const { return value; }

constexpr Id::ValueType Id::NullValue() { return entt::null; }

constexpr Id Id::Null() { return Id{NullValue()}; }

constexpr StringId::StringId(ValueType v) : BasicId<StringId, ValueType>{v} {}

constexpr StringId::ValueType StringId::NullValue() { return entt::hashed_string{}; }

constexpr StringId StringId::Null() { return StringId{NullValue()}; }
} // namespace Engine

template <typename FormatContext>
auto fmt::formatter<Engine::Id>::format(const Engine::Id &id, FormatContext &context) const
{
    return fmt::formatter<entt::id_type>::format(id.value, context);
}

template <typename FormatContext>
auto fmt::formatter<Engine::StringId>::format(const Engine::StringId &id, FormatContext &context) const
{
    return fmt::formatter<std::string_view>::format(std::string_view(id.value.data(), id.value.size()), context);
}
