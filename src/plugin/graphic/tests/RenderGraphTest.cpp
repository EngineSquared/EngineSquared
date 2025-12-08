#include <gtest/gtest.h>

#include "Graphic.hpp"
#include "RenderingPipeline.hpp"
#include "exception/RenderPassSortError.hpp"
#include "resource/RenderGraph.hpp"
#include "resource/SingleExecutionRenderPass.hpp"
#include "utils/ConfigureHeadlessGraphics.hpp"
#include "utils/ThrowErrorIfGraphicalErrorHappened.hpp"

// Mock RenderPass for testing
class MockRenderPass : public Graphic::Resource::ARenderPass {
  public:
    explicit MockRenderPass() : ARenderPass("MockRenderPass"), executeCount(0) {}
    explicit MockRenderPass(std::string_view name) : ARenderPass(name), executeCount(0) {}

    void Execute(Engine::Core &core) override
    {
        executeCount++;
        executionOrder.push_back(GetName());
    }

    int executeCount;
    static std::vector<std::string> executionOrder;
};

std::vector<std::string> MockRenderPass::executionOrder;

// Test fixture for RenderGraph tests
class RenderGraphTest : public ::testing::Test {
  protected:
    void SetUp() override { MockRenderPass::executionOrder.clear(); }

    void TearDown() override { MockRenderPass::executionOrder.clear(); }
};

TEST_F(RenderGraphTest, AddRenderPass)
{
    Graphic::Resource::RenderGraph graph;

    MockRenderPass pass1{};
    graph.Add("pass1", std::move(pass1));

    EXPECT_TRUE(graph.Contains("pass1"));
}

TEST_F(RenderGraphTest, AddMultipleRenderPasses)
{
    Graphic::Resource::RenderGraph graph;

    MockRenderPass pass1{};
    MockRenderPass pass2{};
    MockRenderPass pass3{};

    graph.Add("pass1", std::move(pass1));
    graph.Add("pass2", std::move(pass2));
    graph.Add("pass3", std::move(pass3));

    EXPECT_TRUE(graph.Contains("pass1"));
    EXPECT_TRUE(graph.Contains("pass2"));
    EXPECT_TRUE(graph.Contains("pass3"));
}

TEST_F(RenderGraphTest, AddDuplicateRenderPass)
{
    Graphic::Resource::RenderGraph graph;

    MockRenderPass pass1{};
    MockRenderPass pass1_duplicate{};

    graph.Add("pass1", std::move(pass1));
    graph.Add("pass1", std::move(pass1_duplicate));

    EXPECT_TRUE(graph.Contains("pass1"));
}

TEST_F(RenderGraphTest, RemoveRenderPass)
{
    Graphic::Resource::RenderGraph graph;

    MockRenderPass pass1{};
    graph.Add("pass1", std::move(pass1));

    EXPECT_TRUE(graph.Contains("pass1"));

    graph.Remove("pass1");

    EXPECT_FALSE(graph.Contains("pass1"));
}

TEST_F(RenderGraphTest, RemoveNonExistentRenderPass)
{
    Graphic::Resource::RenderGraph graph;

    EXPECT_FALSE(graph.Contains("nonexistent"));

    // Should not throw, just log a warning
    EXPECT_NO_THROW(graph.Remove("nonexistent"));
}

TEST_F(RenderGraphTest, ContainsRenderPass)
{
    Graphic::Resource::RenderGraph graph;

    MockRenderPass pass1{};
    graph.Add("pass1", std::move(pass1));

    EXPECT_TRUE(graph.Contains("pass1"));
    EXPECT_FALSE(graph.Contains("pass2"));
}

TEST_F(RenderGraphTest, SetDependency)
{
    Graphic::Resource::RenderGraph graph;

    MockRenderPass pass1{};
    MockRenderPass pass2{};

    graph.Add("pass1", std::move(pass1));
    graph.Add("pass2", std::move(pass2));

    EXPECT_NO_THROW(graph.SetDependency("pass1", "pass2"));
}

TEST_F(RenderGraphTest, SetDependencyWithNonExistentBefore)
{
    Graphic::Resource::RenderGraph graph;

    MockRenderPass pass2{};
    graph.Add("pass2", std::move(pass2));

    // Should not throw, just log a warning
    EXPECT_NO_THROW(graph.SetDependency("nonexistent", "pass2"));
}

TEST_F(RenderGraphTest, SetDependencyWithNonExistentAfter)
{
    Graphic::Resource::RenderGraph graph;

    MockRenderPass pass1{};
    graph.Add("pass1", std::move(pass1));

    // Should not throw, just log a warning
    EXPECT_NO_THROW(graph.SetDependency("pass1", "nonexistent"));
}

TEST_F(RenderGraphTest, SetDependencyWithBothNonExistent)
{
    Graphic::Resource::RenderGraph graph;

    // Should not throw, just log warnings
    EXPECT_NO_THROW(graph.SetDependency("nonexistent1", "nonexistent2"));
}

TEST_F(RenderGraphTest, ExecuteWithoutDependencies)
{
    Engine::Core core;
    core.AddPlugins<Graphic::Plugin>();
    core.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);
    core.RegisterSystem<RenderingPipeline::Init>(Graphic::Tests::Utils::ConfigureHeadlessGraphics,
                                                 Graphic::Tests::Utils::ThrowErrorIfGraphicalErrorHappened);
    core.RunSystems();

    Graphic::Resource::RenderGraph graph;

    MockRenderPass pass1{};
    MockRenderPass pass2{};
    MockRenderPass pass3{};

    graph.Add("pass1", std::move(pass1));
    graph.Add("pass2", std::move(pass2));
    graph.Add("pass3", std::move(pass3));

    EXPECT_NO_THROW(graph.Execute(core));

    // All passes should be executed
    EXPECT_EQ(MockRenderPass::executionOrder.size(), 3);
}

TEST_F(RenderGraphTest, ExecuteWithLinearDependencies)
{
    Engine::Core core;
    core.AddPlugins<Graphic::Plugin>();
    core.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);
    core.RegisterSystem<RenderingPipeline::Init>(Graphic::Tests::Utils::ConfigureHeadlessGraphics,
                                                 Graphic::Tests::Utils::ThrowErrorIfGraphicalErrorHappened);
    core.RunSystems();

    Graphic::Resource::RenderGraph graph;

    MockRenderPass pass1("pass1name");
    MockRenderPass pass2("pass2name");
    MockRenderPass pass3("pass3name");

    graph.Add("pass1", std::move(pass1));
    graph.Add("pass2", std::move(pass2));
    graph.Add("pass3", std::move(pass3));

    // Create linear dependency: pass1 -> pass2 -> pass3
    graph.SetDependency("pass1", "pass2");
    graph.SetDependency("pass2", "pass3");

    EXPECT_NO_THROW(graph.Execute(core));

    ASSERT_EQ(MockRenderPass::executionOrder.size(), 3);
    EXPECT_EQ(MockRenderPass::executionOrder[0], "pass1name");
    EXPECT_EQ(MockRenderPass::executionOrder[1], "pass2name");
    EXPECT_EQ(MockRenderPass::executionOrder[2], "pass3name");
}

TEST_F(RenderGraphTest, ExecuteWithComplexDependencies)
{
    Engine::Core core;
    core.AddPlugins<Graphic::Plugin>();
    core.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);
    core.RegisterSystem<RenderingPipeline::Init>(Graphic::Tests::Utils::ConfigureHeadlessGraphics,
                                                 Graphic::Tests::Utils::ThrowErrorIfGraphicalErrorHappened);
    core.RunSystems();

    Graphic::Resource::RenderGraph graph;

    MockRenderPass pass1("pass1name");
    MockRenderPass pass2("pass2name");
    MockRenderPass pass3("pass3name");
    MockRenderPass pass4("pass4name");

    graph.Add("pass1", std::move(pass1));
    graph.Add("pass2", std::move(pass2));
    graph.Add("pass3", std::move(pass3));
    graph.Add("pass4", std::move(pass4));

    // Create diamond dependency:
    //     pass1
    //    /     \
    // pass2   pass3
    //    \     /
    //     pass4
    graph.SetDependency("pass1", "pass2");
    graph.SetDependency("pass1", "pass3");
    graph.SetDependency("pass2", "pass4");
    graph.SetDependency("pass3", "pass4");

    EXPECT_NO_THROW(graph.Execute(core));

    ASSERT_EQ(MockRenderPass::executionOrder.size(), 4);
    EXPECT_EQ(MockRenderPass::executionOrder[0], "pass1name");
    // pass2 and pass3 can be in any order
    EXPECT_TRUE(
        (MockRenderPass::executionOrder[1] == "pass2name" && MockRenderPass::executionOrder[2] == "pass3name") ||
        (MockRenderPass::executionOrder[1] == "pass3name" && MockRenderPass::executionOrder[2] == "pass2name"));
    EXPECT_EQ(MockRenderPass::executionOrder[3], "pass4name");
}

TEST_F(RenderGraphTest, ExecuteWithCyclicDependency)
{
    Engine::Core core;
    core.AddPlugins<Graphic::Plugin>();
    core.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);
    core.RegisterSystem<RenderingPipeline::Init>(Graphic::Tests::Utils::ConfigureHeadlessGraphics,
                                                 Graphic::Tests::Utils::ThrowErrorIfGraphicalErrorHappened);
    core.RunSystems();

    Graphic::Resource::RenderGraph graph;

    MockRenderPass pass1{};
    MockRenderPass pass2{};
    MockRenderPass pass3{};

    graph.Add("pass1", std::move(pass1));
    graph.Add("pass2", std::move(pass2));
    graph.Add("pass3", std::move(pass3));

    // Create cyclic dependency: pass1 -> pass2 -> pass3 -> pass1
    graph.SetDependency("pass1", "pass2");
    graph.SetDependency("pass2", "pass3");
    graph.SetDependency("pass3", "pass1");

    EXPECT_THROW(graph.Execute(core), Graphic::Exception::RenderPassSortError);
}

TEST_F(RenderGraphTest, RemoveRenderPassWithDependencies)
{
    Graphic::Resource::RenderGraph graph;

    MockRenderPass pass1{};
    MockRenderPass pass2{};
    MockRenderPass pass3{};

    graph.Add("pass1", std::move(pass1));
    graph.Add("pass2", std::move(pass2));
    graph.Add("pass3", std::move(pass3));

    // pass1 -> pass2 -> pass3
    graph.SetDependency("pass1", "pass2");
    graph.SetDependency("pass2", "pass3");

    // Remove pass2
    graph.Remove("pass2");

    EXPECT_FALSE(graph.Contains("pass2"));
    EXPECT_TRUE(graph.Contains("pass1"));
    EXPECT_TRUE(graph.Contains("pass3"));
}

TEST_F(RenderGraphTest, ExecuteEmptyGraph)
{
    Engine::Core core;
    core.AddPlugins<Graphic::Plugin>();
    core.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);
    core.RegisterSystem<RenderingPipeline::Init>(Graphic::Tests::Utils::ConfigureHeadlessGraphics,
                                                 Graphic::Tests::Utils::ThrowErrorIfGraphicalErrorHappened);
    core.RunSystems();

    Graphic::Resource::RenderGraph graph;

    EXPECT_NO_THROW(graph.Execute(core));
    EXPECT_EQ(MockRenderPass::executionOrder.size(), 0);
}

TEST_F(RenderGraphTest, MultipleDependenciesOnSamePass)
{
    Engine::Core core;
    core.AddPlugins<Graphic::Plugin>();
    core.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);
    core.RegisterSystem<RenderingPipeline::Init>(Graphic::Tests::Utils::ConfigureHeadlessGraphics,
                                                 Graphic::Tests::Utils::ThrowErrorIfGraphicalErrorHappened);
    core.RunSystems();

    Graphic::Resource::RenderGraph graph;

    MockRenderPass pass1("pass1name");
    MockRenderPass pass2{};
    MockRenderPass pass3{};
    MockRenderPass pass4{};

    graph.Add("pass1", std::move(pass1));
    graph.Add("pass2", std::move(pass2));
    graph.Add("pass3", std::move(pass3));
    graph.Add("pass4", std::move(pass4));

    // Multiple passes depend on pass1
    graph.SetDependency("pass1", "pass2");
    graph.SetDependency("pass1", "pass3");
    graph.SetDependency("pass1", "pass4");

    EXPECT_NO_THROW(graph.Execute(core));

    ASSERT_EQ(MockRenderPass::executionOrder.size(), 4);
    EXPECT_EQ(MockRenderPass::executionOrder[0], "pass1name");
}

TEST_F(RenderGraphTest, ReExecuteAfterModification)
{
    Engine::Core core;
    core.AddPlugins<Graphic::Plugin>();
    core.SetErrorPolicyForAllSchedulers(Engine::Scheduler::SchedulerErrorPolicy::Nothing);
    core.RegisterSystem<RenderingPipeline::Init>(Graphic::Tests::Utils::ConfigureHeadlessGraphics,
                                                 Graphic::Tests::Utils::ThrowErrorIfGraphicalErrorHappened);
    core.RunSystems();

    Graphic::Resource::RenderGraph graph;

    MockRenderPass pass1{};
    MockRenderPass pass2{};

    graph.Add("pass1", std::move(pass1));
    graph.Add("pass2", std::move(pass2));

    graph.Execute(core);

    EXPECT_EQ(MockRenderPass::executionOrder.size(), 2);

    // Add a new pass and set dependency
    MockRenderPass pass3{};
    graph.Add("pass3", std::move(pass3));
    graph.SetDependency("pass2", "pass3");

    MockRenderPass::executionOrder.clear();

    graph.Execute(core);

    ASSERT_EQ(MockRenderPass::executionOrder.size(), 3);
}
