#include <gtest/gtest.h>

#include "Engine.hpp"
#include "RenderingPipeline.hpp"

struct History {
    std::vector<std::string> messages;
};

#define ADD_SCHEDULER_TO_HISTORY(scheduler)                                                                            \
    core.RegisterSystem<scheduler>([](ES::Engine::Core &c) {                                                           \
        auto &history = c.GetResource<History>();                                                                      \
        history.messages.emplace_back(#scheduler);                                                                     \
    });

TEST(RenderingPipeline, CasualUse)
{
    Engine::Core core;

    core.RegisterResource<History>(History{});

    core.AddPlugins<RenderingPipeline::Plugin>();

    ADD_SCHEDULER_TO_HISTORY(ES::Plugin::RenderingPipeline::Init)
    ADD_SCHEDULER_TO_HISTORY(ES::Plugin::RenderingPipeline::Setup)
    ADD_SCHEDULER_TO_HISTORY(ES::Engine::Scheduler::Startup)
    ADD_SCHEDULER_TO_HISTORY(ES::Plugin::RenderingPipeline::PreUpdate)
    ADD_SCHEDULER_TO_HISTORY(ES::Engine::Scheduler::Update)
    ADD_SCHEDULER_TO_HISTORY(ES::Plugin::RenderingPipeline::Preparation)
    ADD_SCHEDULER_TO_HISTORY(ES::Plugin::RenderingPipeline::Extraction)
    ADD_SCHEDULER_TO_HISTORY(ES::Plugin::RenderingPipeline::PipelineCreation)
    ADD_SCHEDULER_TO_HISTORY(ES::Plugin::RenderingPipeline::Batching)
    ADD_SCHEDULER_TO_HISTORY(ES::Plugin::RenderingPipeline::PipelineExecution)
    ADD_SCHEDULER_TO_HISTORY(ES::Plugin::RenderingPipeline::Submission)
    ADD_SCHEDULER_TO_HISTORY(ES::Plugin::RenderingPipeline::Presentation)
    ADD_SCHEDULER_TO_HISTORY(ES::Engine::Scheduler::Shutdown)

    core.RunSystems();

    auto &history = core.GetResource<History>();
    EXPECT_EQ(history.messages.size(), 13);
    EXPECT_EQ(history.messages[0], "ES::Plugin::RenderingPipeline::Init");
    EXPECT_EQ(history.messages[1], "ES::Plugin::RenderingPipeline::Setup");
    EXPECT_EQ(history.messages[2], "ES::Engine::Scheduler::Startup");

    EXPECT_EQ(history.messages[3], "ES::Plugin::RenderingPipeline::PreUpdate");
    EXPECT_EQ(history.messages[4], "ES::Engine::Scheduler::Update");
    EXPECT_EQ(history.messages[5], "ES::Plugin::RenderingPipeline::Preparation");
    EXPECT_EQ(history.messages[6], "ES::Plugin::RenderingPipeline::Extraction");
    EXPECT_EQ(history.messages[7], "ES::Plugin::RenderingPipeline::PipelineCreation");
    EXPECT_EQ(history.messages[8], "ES::Plugin::RenderingPipeline::Batching");
    EXPECT_EQ(history.messages[9], "ES::Plugin::RenderingPipeline::PipelineExecution");
    EXPECT_EQ(history.messages[10], "ES::Plugin::RenderingPipeline::Submission");
    EXPECT_EQ(history.messages[11], "ES::Plugin::RenderingPipeline::Presentation");

    EXPECT_EQ(history.messages[12], "ES::Engine::Scheduler::Shutdown");
}
