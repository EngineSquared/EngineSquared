#include <gtest/gtest.h>

#include "core/Core.hpp"
#include "plugin/APlugin.hpp"
#include "scheduler/Startup.hpp"

struct ResourceTest {
    std::vector<std::string> data;
};

class PluginTestA : public Engine::APlugin {
  public:
    explicit PluginTestA(Engine::Core &core)
        : Engine::APlugin(core){
              // empty
          };
    ~PluginTestA() = default;

    void Bind() final
    {
        RegisterResource<ResourceTest>({});
        RegisterSystems<Engine::Scheduler::Update>([](Engine::Core &core) {
            auto &resource = core.GetResource<ResourceTest>();
            resource.data.emplace_back("PluginTestA::Bind");
        });
    }
};

class PluginTestB : public Engine::APlugin {
  public:
    explicit PluginTestB(Engine::Core &core)
        : Engine::APlugin(core){
              // empty
          };
    ~PluginTestB() = default;

    void Bind() final
    {
        RequirePlugins<PluginTestA>();
        RegisterSystems<Engine::Scheduler::Update>([](Engine::Core &core) {
            auto &resource = core.GetResource<ResourceTest>();
            resource.data.emplace_back("PluginTestB::Bind");
        });
    }
};

class PluginTestC : public Engine::APlugin {
  public:
    explicit PluginTestC(Engine::Core &core)
        : Engine::APlugin(core){
              // empty
          };
    ~PluginTestC() = default;

    void Bind() final
    {
        auto systemIdTuple = RegisterSystems<Engine::Scheduler::Update>([](Engine::Core &core) {
            auto &resource = core.GetResource<ResourceTest>();
            resource.data.emplace_back("PluginTestC::UpdateLambdaSystem");
        });
        systemId = std::get<0>(systemIdTuple);
    }

    // For testing purposes, the systemId is exposed in public. Don't know if it's a good idea in production code.
    FunctionUtils::FunctionID systemId;
};

TEST(Plugin, CasualUse)
{
    Engine::Core core;
    core.AddPlugins<PluginTestB>();
    core.RunSystems();

    auto &resource = core.GetResource<ResourceTest>();
    ASSERT_EQ(resource.data.size(), 2);
    ASSERT_EQ(resource.data[0], "PluginTestA::Bind");
    ASSERT_EQ(resource.data[1], "PluginTestB::Bind");
}

