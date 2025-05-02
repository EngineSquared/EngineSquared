#pragma once

#ifdef ES_DEBUG
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#else
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif

#include "spdlog/spdlog.h"

namespace ES::Utils::Log {

using Level = spdlog::level::level_enum;

template <typename T> inline void Debug(const T &msg) noexcept { spdlog::debug(msg); };

template <typename T> inline void Info(const T &msg) noexcept { spdlog::info(msg); };

template <typename T> inline void Warn(const T &msg) noexcept { spdlog::warn(msg); };

template <typename T> inline void Error(const T &msg) noexcept { spdlog::error(msg); };

template <typename T> inline void Critical(const T &msg) noexcept { spdlog::critical(msg); };

template <typename T> inline void Trace(const T &msg) noexcept { spdlog::trace(msg); };

template <typename T> inline void Log(Level level, const T &msg) noexcept
{
    if (level == Level::info)
        Log::Info(msg);
    else if (level == Level::warn)
        Log::Warn(msg);
    else if (level == Level::err)
        Log::Error(msg);
    else if (level == Level::critical)
        Log::Critical(msg);
    else if (level == Level::debug)
        Log::Debug(msg);
    else
        Log::Trace(msg);
};

inline void SetPattern(const std::string &pattern,
                       spdlog::pattern_time_type time_type = spdlog::pattern_time_type::local)
{
    spdlog::set_pattern(pattern, time_type);
};
} // namespace ES::Utils::Log
