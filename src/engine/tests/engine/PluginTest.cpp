#include <gtest/gtest.h>

#include "APlugin.hpp"
#include "Core.hpp"

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

    void Build() final
    {
        RegisterResource<ResourceTest>({});
        AddSystems<ES::Engine::Scheduler::Update>([this](ES::Engine::Core &core) {
            auto &resource = core.GetResource<ResourceTest>();
            resource.data.push_back("PluginTestA::Build");
        });
    }
};

TEST(Plugin, CasualUse)
{
    ES::Engine::Core core;
    core.AddPlugins<PluginTestA>();
    core.RunSystems();

    auto &resource = core.GetResource<ResourceTest>();
    ASSERT_EQ(resource.data.size(), 1);
    ASSERT_EQ(resource.data[0], "PluginTestA::Build");
}
