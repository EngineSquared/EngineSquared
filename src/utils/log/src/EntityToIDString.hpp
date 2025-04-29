#pragma once

#include <string>
#include <fmt/format.h>
#include <bit>

namespace ES::Utils::Log {
template<typename T>
inline std::string EntityToDebugString(const T &entity)
{
    return fmt::format("({}, {})", entity & 0x000FFFFF, entity >> 20);
}
}