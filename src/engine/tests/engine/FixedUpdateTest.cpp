#include <gtest/gtest.h>

#include "Entity.hpp"
#include "FixedUpdate.hpp"
#include "Registry.hpp"

using namespace ES::Engine;
using namespace std::chrono_literals;

TEST(Registry, FixedUpdate)
{
    Registry reg;

    int update_count = 0;

    // using a tick rate of 1/5 instead of the default 1/50
    // because sleep_for is not precise enough

    reg.RegisterSystem<Scheduler::FixedUpdate>([&](Registry &registry) { update_count++; });
    reg.GetScheduler<Scheduler::FixedUpdate>().SetTickRate(1.0 / 5.0);

    // First update just after creating should not run the system
    reg.RunSystems();
    ASSERT_EQ(update_count, 0);

    // Assuming tick rate of 1/5, if we sleep for .2s, we should have 1 update
    std::this_thread::sleep_for(0.2s);
    reg.RunSystems();
    ASSERT_EQ(update_count, 1);

    // If we sleep for .4s, we should have 2 more updates
    std::this_thread::sleep_for(0.4s);
    reg.RunSystems();
    ASSERT_EQ(update_count, 3);

    // If we sleep for .1s, we should have 0 updates, but we accumulate the time
    std::this_thread::sleep_for(0.1s);
    reg.RunSystems();
    ASSERT_EQ(update_count, 3);

    // Now if we sleep back .1s, we should have 1 more update
    std::this_thread::sleep_for(0.1s);
    reg.RunSystems();
    ASSERT_EQ(update_count, 4);

    // If we sleep for .5s, we should have 2 more updates and accumulate the time
    std::this_thread::sleep_for(0.5s);
    reg.RunSystems();
    ASSERT_EQ(update_count, 6);
    std::this_thread::sleep_for(0.1s);
    reg.RunSystems();
    ASSERT_EQ(update_count, 7);
}
