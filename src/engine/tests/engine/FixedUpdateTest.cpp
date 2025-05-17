#include <gtest/gtest.h>

#include "Core.hpp"
#include "Entity.hpp"
#include "FixedTimeUpdate.hpp"
#include "Time.hpp"

using namespace ES::Engine;
using namespace std::chrono_literals;

TEST(Core, FixedTimeUpdate)
{
    Core core;

    float elapsedTime = 0.f;

    core.RegisterSystem<Scheduler::Update>(
        [&elapsedTime](Core &c) {
            c.GetResource<ES::Engine::Resource::Time>()._elapsedTime = elapsedTime;
        });

    int update_count = 0;

    // using a tick rate of 1/5 instead of the default 1/50
    // because sleep_for is not precise enough

    core.RegisterSystem<Scheduler::FixedTimeUpdate>([&update_count](const Core &) { update_count++; });
    core.GetScheduler<Scheduler::FixedTimeUpdate>().SetTickRate(1.0 / 5.0);

    // First update just after creating should not run the system
    core.RunSystems();
    ASSERT_EQ(update_count, 0);

    // Assuming tick rate of 1/5, if we sleep for .2s, we should have 1 update
    elapsedTime = 0.2f;
    core.RunSystems();
    ASSERT_EQ(update_count, 1);

    // If we sleep for .4s, we should have 2 more updates
    elapsedTime = 0.4f;
    core.RunSystems();
    ASSERT_EQ(update_count, 3);

    // If we sleep for .1s, we should have 0 updates, but we accumulate the time
    elapsedTime = 0.1f;
    core.RunSystems();
    ASSERT_EQ(update_count, 3);

    // Now if we sleep back .1s, we should have 1 more update
    elapsedTime = 0.1f;
    core.RunSystems();
    ASSERT_EQ(update_count, 4);

    // If we sleep for .5s, we should have 2 more updates and accumulate the time
    elapsedTime = 0.5f;
    core.RunSystems();
    ASSERT_EQ(update_count, 6);

    elapsedTime = 0.100001f; // Adding a little bit more to avoid floating point precision issues
    core.RunSystems();
    ASSERT_EQ(update_count, 7);
}
