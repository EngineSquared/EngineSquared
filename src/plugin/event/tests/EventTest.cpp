#include <gtest/gtest.h>

#include "core/Core.hpp"
#include "plugin/PluginEvent.hpp"
#include "resource/EventManager.hpp"
#include "resource/Time.hpp"
#include "scheduler/FixedTimeUpdate.hpp"

struct TestResource {
    int value = 0;
};

struct TestEvent {
    int value;
};

TEST(Event, integration_test)
{
    Engine::Core core;

    core.AddPlugins<Event::Plugin>();
    core.RegisterResource<TestResource>(TestResource{});

    auto &eventManager = core.GetResource<Event::Resource::EventManager>();

    auto callbackID = eventManager.RegisterCallback<TestEvent>([&core](const TestEvent &event) {
        auto &res = core.GetResource<TestResource>();
        res.value = event.value;
    });

    eventManager.PushEvent(TestEvent{42});

    auto &res = core.GetResource<TestResource>();
    EXPECT_EQ(res.value, 42);

    res.value = 0;
    eventManager.UnregisterCallback<TestEvent>(callbackID);
    eventManager.PushEvent(TestEvent{42});

    EXPECT_EQ(res.value, 0);
}

TEST(Event, multi_scheduler_test)
{
    Engine::Core core;

    core.AddPlugins<Event::Plugin>();
    core.RegisterResource<TestResource>(TestResource{});

    auto &eventManager = core.GetResource<Event::Resource::EventManager>();

    eventManager.RegisterCallback<TestEvent, Engine::Scheduler::Update>([&core](const TestEvent &event) {
        auto &res = core.GetResource<TestResource>();
        res.value += event.value;
    });

    eventManager.RegisterCallback<TestEvent, Engine::Scheduler::FixedTimeUpdate>(
        [&core](const TestEvent &event) {
            auto &res = core.GetResource<TestResource>();
            res.value += event.value * 2;
        });

    eventManager.PushEvent(TestEvent{10});

    eventManager.ProcessEvents<Engine::Scheduler::Update>();

    auto &res = core.GetResource<TestResource>();
    EXPECT_EQ(res.value, 10);
    eventManager.ProcessEvents<Engine::Scheduler::FixedTimeUpdate>();
    EXPECT_EQ(res.value, 30);

    res.value = 0;
    eventManager.PushEvent(TestEvent{5});

    core.GetResource<Engine::Resource::Time>()._elapsedTime = 0.001f;
    core.RunSystems();
    EXPECT_EQ(res.value, 5);

    core.RegisterSystem<Engine::Scheduler::Update>(
        [&](Engine::Core &c) { c.GetResource<Engine::Resource::Time>()._elapsedTime = 1.0f; });

    core.RunSystems();

    eventManager.PushEvent(TestEvent{1});
    res.value = 0;

    core.RegisterSystem<Engine::Scheduler::Update>(
        [&](Engine::Core &c) { c.GetResource<Engine::Resource::Time>()._elapsedTime = 0.0001f; });
    core.RunSystems();
    EXPECT_EQ(res.value, 1);

    core.RegisterSystem<Engine::Scheduler::Update>(
        [&](Engine::Core &c) { c.GetResource<Engine::Resource::Time>()._elapsedTime = 1.0f; });
    core.RunSystems();

    EXPECT_EQ(res.value, 3);
}
