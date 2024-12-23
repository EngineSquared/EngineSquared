#include <gtest/gtest.h>

#include <array>
#include <chrono>

#include "Entity.hpp"
#include "Registry.hpp"
#include "RelativeTimeUpdate.hpp"

using namespace ES::Engine;
using namespace std::chrono_literals;

TEST(Registry, RelativeTimeUpdateBasic)
{
    Registry reg;

    int update_count = 0;

    float deltaTime1 = 0.0f;
    float deltaTime2 = 0.0f;

    // Relative time update uses the highest precision available given how often it is called
    // it only uses multiple updates if the time difference is greater than the target tick rate
    reg.RegisterSystem<Scheduler::RelativeTimeUpdate>([&](const Registry &registry) {
        if (deltaTime1 == 0.0f)
        {
            deltaTime1 = reg.GetScheduler<Scheduler::RelativeTimeUpdate>().GetCurrentDeltaTime();
        }
        else
        {
            deltaTime2 = reg.GetScheduler<Scheduler::RelativeTimeUpdate>().GetCurrentDeltaTime();
        }
    });
    reg.GetScheduler<Scheduler::RelativeTimeUpdate>().SetTargetTickRate(1.0 / 5.0);

    reg.RunSystems();
    std::this_thread::sleep_for(0.05s);
    reg.RunSystems();
    std::this_thread::sleep_for(0.1s);
    reg.RunSystems();
    ASSERT_GT(deltaTime2, deltaTime1);
}

TEST(Registry, RelativeTimeUpdateSubsteps)
{
    Registry reg;

    int update_count = 0;

    std::array<float, 5> deltaTimes = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    // If called with a delta time > target tick rate, it should run multiple times, with the remainder
    reg.RegisterSystem<Scheduler::RelativeTimeUpdate>([&deltaTimes](Registry &registry) {
        for (int i = 0; i < 5; i++)
        {
            if (deltaTimes[i] == 0.0f)
            {
                deltaTimes[i] = registry.GetScheduler<Scheduler::RelativeTimeUpdate>().GetCurrentDeltaTime();
                break;
            }
        }
    });
    reg.GetScheduler<Scheduler::RelativeTimeUpdate>().SetTargetTickRate(1.0 / 5.0);

    reg.RunSystems();
    std::this_thread::sleep_for(0.5s);
    reg.RunSystems();
    ASSERT_EQ(deltaTimes[0], 0.2f);
    ASSERT_EQ(deltaTimes[1], 0.2f);
    ASSERT_GT(deltaTimes[2], 0.0f);
    ASSERT_LT(deltaTimes[2], 0.2f);
}
