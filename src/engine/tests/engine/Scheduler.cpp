#include <gtest/gtest.h>

#include "Core.hpp"
#include "Entity.hpp"
#include "Startup.hpp"

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

TEST(SchedulerContainer, CurrentScheduler)
{
    Core core;
    core.RegisterResource<ResourceTest>(ResourceTest());
    auto &data = core.GetResource<ResourceTest>().data;
    core.RegisterSystem<Scheduler::Startup>([](Core &c) { c.GetResource<ResourceTest>().data.push_back(1); });
    core.RegisterSystem<Scheduler::Update>([](Core &c) { c.GetResource<ResourceTest>().data.push_back(2); });
    core.RegisterSystem<Scheduler::Shutdown>([](Core &c) { c.GetResource<ResourceTest>().data.push_back(3); });

    core.RunSystems();
    core.RunSystems();

    ASSERT_EQ(data.size(), 5);
    ASSERT_EQ(data[0], 1);
    ASSERT_EQ(data[1], 2);
    ASSERT_EQ(data[2], 3);
    ASSERT_EQ(data[3], 2);
    ASSERT_EQ(data[4], 3);
}

TEST(SchedulerErrorPolicy, Silent)
{
    testing::internal::CaptureStdout();
    testing::internal::CaptureStderr();

    Core core;
    core.RegisterResource<ResourceTest>(ResourceTest());
    auto &data = core.GetResource<ResourceTest>().data;
    auto &startup = core.GetScheduler<Scheduler::Startup>();
    startup.SetErrorPolicy(Scheduler::SchedulerErrorPolicy::Silent);
    core.RegisterSystem<Scheduler::Startup>([](Core &c) { c.GetResource<ResourceTest>().data.push_back(1); });
    core.RegisterSystem<Scheduler::Startup>([](Core &c) { throw std::runtime_error("Error"); });
    core.RegisterSystem<Scheduler::Startup>([](Core &c) { c.GetResource<ResourceTest>().data.push_back(3); });
    core.RegisterSystem<Scheduler::Update>([](Core &c) { c.GetResource<ResourceTest>().data.push_back(4); });

    core.RunSystems();

    auto output = testing::internal::GetCapturedStdout();
    auto error = testing::internal::GetCapturedStderr();
    ASSERT_EQ(data.size(), 3);
    ASSERT_EQ(data[0], 1);
    ASSERT_EQ(data[1], 3);
    ASSERT_EQ(data[2], 4);
    ASSERT_TRUE(output.empty());
    ASSERT_TRUE(error.empty());
}

TEST(SchedulerErrorPolicy, LogAndContinue)
{
    Core core;
    core.RegisterResource<ResourceTest>(ResourceTest());
    auto &data = core.GetResource<ResourceTest>().data;
    auto &startup = core.GetScheduler<Scheduler::Startup>();
    startup.SetErrorPolicy(Scheduler::SchedulerErrorPolicy::LogAndContinue);
    core.RegisterSystem<Scheduler::Startup>([](Core &c) { c.GetResource<ResourceTest>().data.push_back(1); });
    core.RegisterSystem<Scheduler::Startup>([](Core &c) { throw std::runtime_error("Error"); });
    core.RegisterSystem<Scheduler::Startup>([](Core &c) { c.GetResource<ResourceTest>().data.push_back(3); });
    core.RegisterSystem<Scheduler::Update>([](Core &c) { c.GetResource<ResourceTest>().data.push_back(4); });

    core.RunSystems();

    ASSERT_EQ(data.size(), 3);
    ASSERT_EQ(data[0], 1);
    ASSERT_EQ(data[1], 3);
    ASSERT_EQ(data[2], 4);
}

TEST(SchedulerErrorPolicy, LogAndFinishScheduler)
{
    Core core;
    core.RegisterResource<ResourceTest>(ResourceTest());
    auto &data = core.GetResource<ResourceTest>().data;
    auto &startup = core.GetScheduler<Scheduler::Startup>();
    startup.SetErrorPolicy(Scheduler::SchedulerErrorPolicy::LogAndFinishScheduler);
    core.RegisterSystem<Scheduler::Startup>([](Core &c) { c.GetResource<ResourceTest>().data.push_back(1); });
    core.RegisterSystem<Scheduler::Startup>([](Core &c) { throw std::runtime_error("Error"); });
    core.RegisterSystem<Scheduler::Startup>([](Core &c) { c.GetResource<ResourceTest>().data.push_back(3); });
    core.RegisterSystem<Scheduler::Update>([](Core &c) { c.GetResource<ResourceTest>().data.push_back(4); });

    core.RunSystems();

    ASSERT_EQ(data.size(), 2);
    ASSERT_EQ(data[0], 1);
    ASSERT_EQ(data[1], 3);
}

TEST(SchedulerErrorPolicy, LogAndStop)
{
    Core core;
    core.RegisterResource<ResourceTest>(ResourceTest());
    auto &data = core.GetResource<ResourceTest>().data;
    auto &startup = core.GetScheduler<Scheduler::Startup>();
    startup.SetErrorPolicy(Scheduler::SchedulerErrorPolicy::LogAndStop);
    core.RegisterSystem<Scheduler::Startup>([](Core &c) { c.GetResource<ResourceTest>().data.push_back(1); });
    core.RegisterSystem<Scheduler::Startup>([](Core &c) { throw std::runtime_error("Error"); });
    core.RegisterSystem<Scheduler::Startup>([](Core &c) { c.GetResource<ResourceTest>().data.push_back(3); });
    core.RegisterSystem<Scheduler::Update>([](Core &c) { c.GetResource<ResourceTest>().data.push_back(4); });

    core.RunSystems();

    ASSERT_EQ(data.size(), 1);
    ASSERT_EQ(data[0], 1);
}
