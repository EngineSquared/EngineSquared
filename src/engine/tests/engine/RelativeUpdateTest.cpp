#include <gtest/gtest.h>

#include <array>
#include <chrono>

#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "resource/Time.hpp"
#include "scheduler/RelativeTimeUpdate.hpp"

using namespace ES::Engine;
using namespace std::chrono_literals;

TEST(Core, RelativeTimeUpdateBasic)
{
    Core core;

    float elapsedTime = 0.f;

    core.RegisterSystem<Scheduler::Update>(
        [&elapsedTime](Core &c) { c.GetResource<ES::Engine::Resource::Time>()._elapsedTime = elapsedTime; });

    float deltaTime1 = 0.0f;
    float deltaTime2 = 0.0f;

    // Relative time update uses the highest precision available given how often it is called
    // it only uses multiple updates if the time difference is greater than the target tick rate
    core.RegisterSystem<Scheduler::RelativeTimeUpdate>([&deltaTime1, &deltaTime2](Core &core_) {
        if (deltaTime1 == 0.0f)
        {
            deltaTime1 = core_.GetScheduler<Scheduler::RelativeTimeUpdate>().GetCurrentDeltaTime();
        }
        else
        {
            deltaTime2 = core_.GetScheduler<Scheduler::RelativeTimeUpdate>().GetCurrentDeltaTime();
        }
    });
    core.GetScheduler<Scheduler::RelativeTimeUpdate>().SetTargetTickRate(1.0 / 5.0);

    core.RunSystems();
    elapsedTime = 0.05f;
    core.RunSystems();
    elapsedTime = 0.1f;
    core.RunSystems();
    ASSERT_GT(deltaTime2, deltaTime1);
}

TEST(Core, RelativeTimeUpdateSubsteps)
{
    Core core;

    float elapsedTime = 0.f;

    core.RegisterSystem<Scheduler::Update>(
        [&elapsedTime](Core &c) { c.GetResource<ES::Engine::Resource::Time>()._elapsedTime = elapsedTime; });

    std::array<float, 5> deltaTimes = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    // If called with a delta time > target tick rate, it should run multiple times, with the remainder
    core.RegisterSystem<Scheduler::RelativeTimeUpdate>([&deltaTimes](Core &core_) {
        for (int i = 0; i < 5; i++)
        {
            if (deltaTimes[i] == 0.0f)
            {
                deltaTimes[i] = core_.GetScheduler<Scheduler::RelativeTimeUpdate>().GetCurrentDeltaTime();
                break;
            }
        }
    });
    core.GetScheduler<Scheduler::RelativeTimeUpdate>().SetTargetTickRate(1.0 / 5.0);

    core.RunSystems();
    elapsedTime = 0.5f;
    core.RunSystems();
    ASSERT_EQ(deltaTimes[0], 0.2f);
    ASSERT_EQ(deltaTimes[1], 0.2f);
    ASSERT_GT(deltaTimes[2], 0.0f);
    ASSERT_LT(deltaTimes[2], 0.2f);
}
