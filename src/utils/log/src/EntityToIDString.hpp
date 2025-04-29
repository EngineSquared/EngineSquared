#pragma once

#include <bit>
#include <fmt/format.h>
#include <string>

namespace ES::Utils::Log {
template <typename T> inline std::string EntityToDebugString(const T &entity)
{
    return fmt::format("({}, {})", entity & 0x000FFFFF, entity >> 20);
}
} // namespace ES::Utils::Log
