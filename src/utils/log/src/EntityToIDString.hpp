#pragma once

#include <bit>
#include <fmt/format.h>
#include <string>

namespace ES::Utils::Log {
template <typename T> inline T GetEntityIndex(const T &entity) { return entity & 0x000FFFFF; }

template <typename T> inline T GetEntityVersion(const T &entity) { return entity >> 20; }

template <typename T> inline std::string EntityToDebugString(const T &entity)
{
    return fmt::format("({}, {})", GetEntityIndex(entity), GetEntityVersion(entity));
}
} // namespace ES::Utils::Log
