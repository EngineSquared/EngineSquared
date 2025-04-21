#include <gtest/gtest.h>

#include "RenderingPipeline.hpp"
#include "Engine.hpp"

struct History
{
    std::vector<std::string> messages;
};

TEST(RenderingPipeline, CasualUse)
{
    ES::Engine::Core core;

    core.RegisterResource<History>(History{});

    core.AddPlugins<ES::Plugin::RenderingPipeline::Plugin>();

    core.RegisterSystem<ES::Plugin::RenderingPipeline::Init>([](ES::Engine::Core &c){
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("Init");
    });
    core.RegisterSystem<ES::Plugin::RenderingPipeline::Setup>([](ES::Engine::Core &c){
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("Setup");
    });
    core.RegisterSystem<ES::Engine::Scheduler::Startup>([](ES::Engine::Core &c){
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("Startup");
    });
    core.RegisterSystem<ES::Plugin::RenderingPipeline::PreUpdate>([](ES::Engine::Core &c){
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("PreUpdate");
    });
    core.RegisterSystem<ES::Engine::Scheduler::Update>([](ES::Engine::Core &c){
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("Update");
    });
    core.RegisterSystem<ES::Plugin::RenderingPipeline::RenderSetup>([](ES::Engine::Core &c){
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("RenderSetup");
    });
    core.RegisterSystem<ES::Plugin::RenderingPipeline::ToGPU>([](ES::Engine::Core &c){
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("ToGPU");
    });
    core.RegisterSystem<ES::Plugin::RenderingPipeline::Draw>([](ES::Engine::Core &c){
        auto &history = c.GetResource<History>();
        history.messages.emplace_back("Draw");
    });
    core.RegisterSystem<ES::Engine::Scheduler::Shutdown>([](ES::Engine::Core &c){
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
