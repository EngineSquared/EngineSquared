#pragma once

#ifdef ES_DEBUG
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#else
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif

#include "spdlog/spdlog.h"

namespace Log {

enum class Level {
    trace,
    debug,
    info,
    warning,
    error,
    critical,
    off,
};

inline constexpr spdlog::level::level_enum ToSpdlogLevel(Level level) noexcept
{
    if (level == Level::trace)
        return spdlog::level::trace;
    else if (level == Level::debug)
        return spdlog::level::debug;
    else if (level == Level::info)
        return spdlog::level::info;
    else if (level == Level::warning)
        return spdlog::level::warn;
    else if (level == Level::error)
        return spdlog::level::err;
    else if (level == Level::critical)
        return spdlog::level::critical;
    else if (level == Level::off)
        return spdlog::level::off;
    else
        return spdlog::level::trace;
}

template <typename T> inline void Debug(const T &msg) noexcept { spdlog::debug(msg); };

template <typename T> inline void Info(const T &msg) noexcept { spdlog::info(msg); };

template <typename T> inline void Warning(const T &msg) noexcept { spdlog::warn(msg); };

template <typename T> inline void Error(const T &msg) noexcept { spdlog::error(msg); };

template <typename T> inline void Critical(const T &msg) noexcept { spdlog::critical(msg); };

template <typename T> inline void Trace(const T &msg) noexcept { spdlog::trace(msg); };

template <typename T> inline void Log(Level level, const T &msg) noexcept
{
    if (level == Level::info)
        Log::Info(msg);
    else if (level == Level::warning)
        Log::Warning(msg);
    else if (level == Level::error)
        Log::Error(msg);
    else if (level == Level::critical)
        Log::Critical(msg);
    else if (level == Level::debug)
        Log::Debug(msg);
    else if (level == Level::off)
        return;
    else
        Log::Trace(msg);
};

inline void SetLevel(Level level) { spdlog::set_level(ToSpdlogLevel(level)); };

inline void SetPattern(const std::string &pattern,
                       spdlog::pattern_time_type time_type = spdlog::pattern_time_type::local)
{
    spdlog::set_pattern(pattern, time_type);
};
} // namespace Log
