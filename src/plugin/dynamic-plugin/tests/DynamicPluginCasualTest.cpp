#include <gtest/gtest.h>

#include "DynamicPlugin.hpp"
#include "scheduler/Startup.hpp"

TEST(DynamicPlugin, SmokeTest)
{
    Engine::Core core;

    core.AddPlugins<DynamicPlugin::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Startup>([](Engine::Core &) {
        // Get Plugin loader to add a new directory to search
    });

    core.RunSystems();
}
