#include <gtest/gtest.h>

#include "Core.hpp"
#include "Entity.hpp"

using namespace ES::Engine;

struct ResourceTest {
    std::vector<int> data;
};

class SchedulerTest1 : public ES::Engine::Scheduler::AScheduler {
  public:
    explicit SchedulerTest1(Core &core) : AScheduler(core){};
    ~SchedulerTest1() final = default;

    void RunSystems() override { _core.GetResource<ResourceTest>().data.push_back(1); }
};

class SchedulerTest2 : public ES::Engine::Scheduler::AScheduler {
  public:
    explicit SchedulerTest2(Core &core) : AScheduler(core){};
    ~SchedulerTest2() final = default;

    void RunSystems() override { _core.GetResource<ResourceTest>().data.push_back(2); }
};

TEST(SchedulerContainer, CasualUse)
{
    {
        Core core;
        core.RegisterResource<ResourceTest>(ResourceTest());
        core.RegisterScheduler<SchedulerTest1>();
        core.RegisterScheduler<SchedulerTest2>();
        core.RunSystems();
        auto &data = core.GetResource<ResourceTest>().data;
        ASSERT_EQ(data.size(), 2);
        ASSERT_EQ(data[0], 1);
        ASSERT_EQ(data[1], 2);
    }
    {
        Core core;
        core.RegisterResource<ResourceTest>(ResourceTest());
        core.RegisterScheduler<SchedulerTest2>();
        core.RegisterScheduler<SchedulerTest1>();
        core.RunSystems();
        auto &data = core.GetResource<ResourceTest>().data;
        ASSERT_EQ(data.size(), 2);
        ASSERT_EQ(data[0], 2);
        ASSERT_EQ(data[1], 1);
    }
}

TEST(SchedulerContainer, DependencyAfter)
{
    Core core;
    core.RegisterResource<ResourceTest>(ResourceTest());
    auto &data = core.GetResource<ResourceTest>().data;
    core.RegisterScheduler<SchedulerTest1>();
    core.RegisterScheduler<SchedulerTest2>();
    core.RunSystems();
    {
        ASSERT_EQ(data.size(), 2);
        ASSERT_EQ(data[0], 1);
        ASSERT_EQ(data[1], 2);
    }
    data.clear();
    core.SetSchedulerAfter<SchedulerTest1, SchedulerTest2>();
    core.RunSystems();
    {
        ASSERT_EQ(data.size(), 2);
        ASSERT_EQ(data[0], 2);
        ASSERT_EQ(data[1], 1);
    }
}

TEST(SchedulerContainer, DependencyBefore)
{
    Core core;
    core.RegisterResource<ResourceTest>(ResourceTest());
    auto &data = core.GetResource<ResourceTest>().data;
    core.RegisterScheduler<SchedulerTest2>();
    core.RegisterScheduler<SchedulerTest1>();
    core.RunSystems();
    {
        ASSERT_EQ(data.size(), 2);
        ASSERT_EQ(data[0], 2);
        ASSERT_EQ(data[1], 1);
    }
    data.clear();
    core.SetSchedulerBefore<SchedulerTest1, SchedulerTest2>();
    core.RunSystems();
    {
        ASSERT_EQ(data.size(), 2);
        ASSERT_EQ(data[0], 1);
        ASSERT_EQ(data[1], 2);
    }
}
