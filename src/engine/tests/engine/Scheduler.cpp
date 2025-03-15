#include <gtest/gtest.h>

#include "Core.hpp"
#include "Entity.hpp"

using namespace ES::Engine;

struct ResourceTest {
    std::vector<int> data;
};

class SchedulerTest1 : public ES::Engine::Scheduler::AScheduler {
  public:
    SchedulerTest1(Core &core) : AScheduler(core){};
    ~SchedulerTest1() = default;

    void RunSystems() override { _registry.GetResource<ResourceTest>().data.push_back(1); }
};

class SchedulerTest2 : public ES::Engine::Scheduler::AScheduler {
  public:
    SchedulerTest2(Core &core) : AScheduler(core){};
    ~SchedulerTest2() = default;

    void RunSystems() override { _registry.GetResource<ResourceTest>().data.push_back(2); }
};

TEST(SchedulerContainer, CasualUse)
{
    {
        Core reg;
        reg.RegisterResource<ResourceTest>(ResourceTest());
        reg.RegisterScheduler<SchedulerTest1>();
        reg.RegisterScheduler<SchedulerTest2>();
        reg.RunSystems();
        auto &data = reg.GetResource<ResourceTest>().data;
        ASSERT_EQ(data.size(), 2);
        ASSERT_EQ(data[0], 1);
        ASSERT_EQ(data[1], 2);
    }
    {
        Core reg;
        reg.RegisterResource<ResourceTest>(ResourceTest());
        reg.RegisterScheduler<SchedulerTest2>();
        reg.RegisterScheduler<SchedulerTest1>();
        reg.RunSystems();
        auto &data = reg.GetResource<ResourceTest>().data;
        ASSERT_EQ(data.size(), 2);
        ASSERT_EQ(data[0], 2);
        ASSERT_EQ(data[1], 1);
    }
}
