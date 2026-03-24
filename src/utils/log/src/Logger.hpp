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
    using enum Log::Level;

    if (level == trace)
        return spdlog::level::trace;
    else if (level == debug)
        return spdlog::level::debug;
    else if (level == info)
        return spdlog::level::info;
    else if (level == warning)
        return spdlog::level::warn;
    else if (level == error)
        return spdlog::level::err;
    else if (level == critical)
        return spdlog::level::critical;
    else if (level == off)
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
    using enum Log::Level;

    if (level == info)
        Log::Info(msg);
    else if (level == warning)
        Log::Warning(msg);
    else if (level == error)
        Log::Error(msg);
    else if (level == critical)
        Log::Critical(msg);
    else if (level == debug)
        Log::Debug(msg);
    else if (level == off)
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
