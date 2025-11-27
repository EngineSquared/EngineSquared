#include <gtest/gtest.h>

#include "core/Core.hpp"
#include "plugin/PluginEvent.hpp"
#include "resource/EventManager.hpp"

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

    auto callbackID = eventManager.RegisterCallback<TestEvent>([](Engine::Core &c, const TestEvent &event) {
        auto &res = c.GetResource<TestResource>();
        res.value = event.value;
    });

    eventManager.PushEvent(TestEvent{42});
    core.RunSystems();

    auto &res = core.GetResource<TestResource>();
    EXPECT_EQ(res.value, 42);

    res.value = 0;
    eventManager.UnregisterCallback<TestEvent>(callbackID);
    eventManager.PushEvent(TestEvent{42});
    core.RunSystems();

    EXPECT_EQ(res.value, 0);
}
