#include <gtest/gtest.h>

#include "Engine.hpp"
#include "RenderingPipeline.hpp"

struct History {
    std::vector<std::string> messages;
};

TEST(RenderingPipeline, CasualUse)
{
    Engine::Core core;

    core.RegisterResource<History>(History{});

    core.AddPlugins<Plugin::RenderingPipeline::Plugin>();

    core.RegisterSystem<Plugin::RenderingPipeline::Init>([](Engine::Core &c) {
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("Init");
    });
    core.RegisterSystem<Plugin::RenderingPipeline::Setup>([](Engine::Core &c) {
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("Setup");
    });
    core.RegisterSystem<Engine::Scheduler::Startup>([](Engine::Core &c) {
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("Startup");
    });
    core.RegisterSystem<Plugin::RenderingPipeline::PreUpdate>([](Engine::Core &c) {
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("PreUpdate");
    });
    core.RegisterSystem<Engine::Scheduler::Update>([](Engine::Core &c) {
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("Update");
    });
    core.RegisterSystem<Plugin::RenderingPipeline::RenderSetup>([](Engine::Core &c) {
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("RenderSetup");
    });
    core.RegisterSystem<Plugin::RenderingPipeline::ToGPU>([](Engine::Core &c) {
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("ToGPU");
    });
    core.RegisterSystem<Plugin::RenderingPipeline::Draw>([](Engine::Core &c) {
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("Draw");
    });
    core.RegisterSystem<Engine::Scheduler::Shutdown>([](Engine::Core &c) {
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("Shutdown");
    });

    core.RunSystems();

    auto &history = core.GetResource<History>();
    EXPECT_EQ(history.messages.size(), 9);
    EXPECT_EQ(history.messages[0], "Init");
    EXPECT_EQ(history.messages[1], "Setup");
    EXPECT_EQ(history.messages[2], "Startup");

    EXPECT_EQ(history.messages[3], "PreUpdate");
    EXPECT_EQ(history.messages[4], "Update");
    EXPECT_EQ(history.messages[5], "RenderSetup");
    EXPECT_EQ(history.messages[6], "ToGPU");
    EXPECT_EQ(history.messages[7], "Draw");

    EXPECT_EQ(history.messages[8], "Shutdown");
}
