#include <gtest/gtest.h>

#include "plugin/APlugin.hpp"
#include "core/Core.hpp"

struct ResourceTest {
    std::vector<std::string> data;
};

class PluginTestA : public ES::Engine::APlugin {
  public:
    explicit PluginTestA(ES::Engine::Core &core)
        : ES::Engine::APlugin(core){
              // empty
          };
    ~PluginTestA() = default;

    void Bind() final
    {
        RegisterResource<ResourceTest>({});
        RegisterSystems<ES::Engine::Scheduler::Update>([](ES::Engine::Core &core) {
            auto &resource = core.GetResource<ResourceTest>();
            resource.data.emplace_back("PluginTestA::Bind");
        });
    }
};

class PluginTestB : public ES::Engine::APlugin {
  public:
    explicit PluginTestB(ES::Engine::Core &core)
        : ES::Engine::APlugin(core){
              // empty
          };
    ~PluginTestB() = default;

    void Bind() final
    {
        RequirePlugins<PluginTestA>();
        RegisterSystems<ES::Engine::Scheduler::Update>([](ES::Engine::Core &core) {
            auto &resource = core.GetResource<ResourceTest>();
            resource.data.emplace_back("PluginTestB::Bind");
        });
    }
};

TEST(Plugin, CasualUse)
{
    ES::Engine::Core core;
    core.AddPlugins<PluginTestB>();
    core.RunSystems();

    auto &resource = core.GetResource<ResourceTest>();
    ASSERT_EQ(resource.data.size(), 2);
    ASSERT_EQ(resource.data[0], "PluginTestA::Bind");
    ASSERT_EQ(resource.data[1], "PluginTestB::Bind");
}
