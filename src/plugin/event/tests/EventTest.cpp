#include <gtest/gtest.h>

#include "resource/EventManager.hpp"
#include "core/Core.hpp"

TEST(Event, casual_use_of_EventManager)
{
    Event::Resource::EventManager eventManager;

    int testVar = 0;

    struct TestEvent {
        int value;
    };

    auto callbackID = eventManager.RegisterCallback<TestEvent>([&testVar](Engine::Core &, const TestEvent &event) {
        EXPECT_EQ(event.value, 42);
        testVar = 42;
    });

    Engine::Core core;

    eventManager.TriggerEvent(core, TestEvent{42});
    EXPECT_EQ(testVar, 42);

    testVar = 0;
    eventManager.UnregisterCallback<TestEvent>(callbackID);
    eventManager.TriggerEvent(core, TestEvent{42});
    EXPECT_EQ(testVar, 0);
}
