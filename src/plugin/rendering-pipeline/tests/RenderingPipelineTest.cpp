#include <gtest/gtest.h>

#include "Engine.hpp"
#include "RenderingPipeline.hpp"

struct History {
    std::vector<std::string> messages;
};

#define LOG_SYSTEM_EXECUTION(systemName)                                    \
    core.RegisterSystem<systemName>([](Engine::Core &c) {                   \
        auto &history = c.GetResource<History>();                           \
        history.messages.emplace_back(#systemName);                         \
    });

TEST(RenderingPipeline, CasualUse)
{
    Engine::Core core;

    core.RegisterResource<History>(History{});

    core.AddPlugins<RenderingPipeline::Plugin>();

    LOG_SYSTEM_EXECUTION(RenderingPipeline::Init);
    LOG_SYSTEM_EXECUTION(RenderingPipeline::Setup);
    LOG_SYSTEM_EXECUTION(Engine::Scheduler::Startup);

    LOG_SYSTEM_EXECUTION(RenderingPipeline::PreUpdate);
    LOG_SYSTEM_EXECUTION(Engine::Scheduler::Update);
    LOG_SYSTEM_EXECUTION(RenderingPipeline::Preparation);
    LOG_SYSTEM_EXECUTION(RenderingPipeline::Extraction);
    LOG_SYSTEM_EXECUTION(RenderingPipeline::PipelineCreation);
    LOG_SYSTEM_EXECUTION(RenderingPipeline::Batching);
    LOG_SYSTEM_EXECUTION(RenderingPipeline::CommandCreation);
    LOG_SYSTEM_EXECUTION(RenderingPipeline::Submission);
    LOG_SYSTEM_EXECUTION(RenderingPipeline::Presentation);

    LOG_SYSTEM_EXECUTION(Engine::Scheduler::Shutdown);

    core.RunSystems();

    auto &history = core.GetResource<History>();
    EXPECT_EQ(history.messages.size(), 13);
    EXPECT_EQ(history.messages[0], "RenderingPipeline::Init");
    EXPECT_EQ(history.messages[1], "RenderingPipeline::Setup");
    EXPECT_EQ(history.messages[2], "Engine::Scheduler::Startup");

    EXPECT_EQ(history.messages[3], "RenderingPipeline::PreUpdate");
    EXPECT_EQ(history.messages[4], "Engine::Scheduler::Update");
    EXPECT_EQ(history.messages[5], "RenderingPipeline::Preparation");
    EXPECT_EQ(history.messages[6], "RenderingPipeline::Extraction");
    EXPECT_EQ(history.messages[7], "RenderingPipeline::PipelineCreation");
    EXPECT_EQ(history.messages[8], "RenderingPipeline::Batching");
    EXPECT_EQ(history.messages[9], "RenderingPipeline::CommandCreation");
    EXPECT_EQ(history.messages[10], "RenderingPipeline::Submission");
    EXPECT_EQ(history.messages[11], "RenderingPipeline::Presentation");

    EXPECT_EQ(history.messages[12], "Engine::Scheduler::Shutdown");
}
