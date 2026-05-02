#pragma once

#ifdef ES_DEBUG
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#else
#    define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_INFO
#endif

#include "spdlog/spdlog.h"

namespace Log {

enum class Level {
    trace = spdlog::level::level_enum::trace,
    debug = spdlog::level::level_enum::debug,
    info = spdlog::level::level_enum::info,
    warning = spdlog::level::level_enum::warn,
    error = spdlog::level::level_enum::err,
    critical = spdlog::level::level_enum::critical,
    off = spdlog::level::level_enum::off,
};

constexpr spdlog::level::level_enum ToSpdlogLevel(Level level) noexcept
{
    return static_cast<spdlog::level::level_enum>(level);
}

template <typename T> inline void Debug(const T &msg) noexcept(false) { spdlog::debug(msg); };

template <typename T> inline void Info(const T &msg) noexcept(false) { spdlog::info(msg); };

template <typename T> inline void Warning(const T &msg) noexcept(false) { spdlog::warn(msg); };

template <typename T> inline void Error(const T &msg) noexcept(false) { spdlog::error(msg); };

template <typename T> inline void Critical(const T &msg) noexcept(false) { spdlog::critical(msg); };

template <typename T> inline void Trace(const T &msg) noexcept(false) { spdlog::trace(msg); };

template <typename T> inline void Log(Level level, const T &msg) noexcept(false)
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
