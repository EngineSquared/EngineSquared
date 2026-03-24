#include <gtest/gtest.h>

#include "Logger.hpp"

namespace {

TEST(LogLevelTest, ToSpdlogLevelMapsAllLevels)
{
    EXPECT_EQ(Log::ToSpdlogLevel(Log::Level::trace), spdlog::level::trace);
    EXPECT_EQ(Log::ToSpdlogLevel(Log::Level::debug), spdlog::level::debug);
    EXPECT_EQ(Log::ToSpdlogLevel(Log::Level::info), spdlog::level::info);
    EXPECT_EQ(Log::ToSpdlogLevel(Log::Level::warning), spdlog::level::warn);
    EXPECT_EQ(Log::ToSpdlogLevel(Log::Level::error), spdlog::level::err);
    EXPECT_EQ(Log::ToSpdlogLevel(Log::Level::critical), spdlog::level::critical);
    EXPECT_EQ(Log::ToSpdlogLevel(Log::Level::off), spdlog::level::off);
}

TEST(LogLevelTest, SetLevelUpdatesGlobalSpdlogLevel)
{
    const auto originalLevel = spdlog::default_logger_raw()->level();

    Log::SetLevel(Log::Level::warning);
    EXPECT_EQ(spdlog::default_logger_raw()->level(), spdlog::level::warn);

    Log::SetLevel(Log::Level::error);
    EXPECT_EQ(spdlog::default_logger_raw()->level(), spdlog::level::err);

    spdlog::set_level(originalLevel);
}

} // namespace
