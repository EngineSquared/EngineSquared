#include <gtest/gtest.h>

#include "Clock.hpp"

using namespace ES::Utils;

TEST(Clock, CasualUsage)
{
    Clock clock;

    ASSERT_EQ(clock.GetElapsedTicks(), 0);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(clock.GetTickRate() * 1.1s);

    ASSERT_EQ(clock.GetElapsedTicks(), 0);

    clock.Update();

    ASSERT_EQ(clock.GetElapsedTicks(), 1);

    clock.Update();

    ASSERT_EQ(clock.GetElapsedTicks(), 0);

    std::this_thread::sleep_for(clock.GetTickRate() * 3.1s);

    clock.Update();

    ASSERT_EQ(clock.GetElapsedTicks(), 3);
}
