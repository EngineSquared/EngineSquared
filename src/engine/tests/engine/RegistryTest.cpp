#include <gtest/gtest.h>

#include "AScheduler.hpp"
#include "Core.hpp"
#include "Entity.hpp"

using namespace ES::Engine;

TEST(Core, CreateEntity)
{
    Core core;

    core.CreateEntity();
}

TEST(Core, Systems)
{
    Core core;

    int x = 42;

    core.RegisterSystem([&x](const Core &) { x = 69; });

    core.RegisterSystem([&x](const Core &) { x = 144; });

    ASSERT_EQ(x, 42);

    core.RunSystems();

    ASSERT_EQ(x, 144);
}

TEST(Core, Resources)
{
    Core core;

    struct Res {
        int x;
    };

    core.RegisterResource<Res>({42});

    core.RegisterSystem([](Core &core) { core.GetResource<Res>().x = 69; });

    ASSERT_EQ(core.GetResource<Res>().x, 42);

    core.RunSystems();

    ASSERT_EQ(core.GetResource<Res>().x, 69);
}

class TestSchedulerA : public Scheduler::AScheduler {
  public:
    using Scheduler::AScheduler::AScheduler;
    void RunSystems() override
    {
        for (auto const &system : this->_systemsList.GetSystems())
        {
            (*system)(_core);
        }
    }
};

class TestSchedulerB : public Scheduler::AScheduler {
  public:
    using Scheduler::AScheduler::AScheduler;
    void RunSystems() override
    {
        for (auto const &system : this->_systemsList.GetSystems())
        {
            (*system)(_core);
        }
    }
};

struct HistoryStorage {
    std::vector<std::string> history;
};

TEST(Core, DefaultScheduler)
{
    Core core;
    core.RegisterResource<HistoryStorage>(HistoryStorage());

    core.RegisterScheduler<TestSchedulerA>();
    core.RegisterScheduler<TestSchedulerB>();

    core.RegisterSystem<TestSchedulerA>([](Core &sCore) {
        auto &history = sCore.GetResource<HistoryStorage>().history;
        history.emplace_back("Starting Scheduler A");
    });

    core.RegisterSystem<TestSchedulerB>([](Core &sCore) {
        auto &history = sCore.GetResource<HistoryStorage>().history;
        history.emplace_back("Starting Scheduler B");
    });

    core.SetDefaultScheduler<TestSchedulerA>();

    core.RegisterSystem([](Core &sCore) {
        auto &history = sCore.GetResource<HistoryStorage>().history;
        history.emplace_back("System Test 1");
    });

    core.SetDefaultScheduler<TestSchedulerB>();

    core.RegisterSystem([](Core &sCore) {
        auto &history = sCore.GetResource<HistoryStorage>().history;
        history.emplace_back("System Test 2");
    });

    core.RunSystems();

    auto &history = core.GetResource<HistoryStorage>().history;

    ASSERT_EQ(history.size(), 4);
    ASSERT_EQ(history[0], "Starting Scheduler A");
    ASSERT_EQ(history[1], "System Test 1");
    ASSERT_EQ(history[2], "Starting Scheduler B");
    ASSERT_EQ(history[3], "System Test 2");
}
