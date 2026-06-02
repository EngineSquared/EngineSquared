#include <gtest/gtest.h>

#include "core/Core.hpp"
#include "plugin/APlugin.hpp"
#include "scheduler/Shutdown.hpp"
#include "scheduler/Startup.hpp"

namespace Example {
struct ResourceTest {
    std::vector<std::string> data;
};

struct ResourceTestCopy {
    std::vector<std::string> data;
};

class PluginTestA : public Engine::APlugin {
  public:
    explicit PluginTestA(Engine::Core &core)
        : Engine::APlugin(core) {
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
        : Engine::APlugin(core) {
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

class PluginTestUnbind : public Engine::APlugin {
  public:
    explicit PluginTestUnbind(Engine::Core &core)
        : Engine::APlugin(core) {
              // empty
          };
    ~PluginTestUnbind() = default;

    void Bind() final
    {
        RegisterResource<ResourceTest>({});
        RegisterSystems<Engine::Scheduler::Startup>([](Engine::Core &core) {
            auto &resource = core.GetResource<ResourceTest>();
            resource.data.emplace_back("PluginTestUnbind::Startup");
            auto &resourceCopy = core.GetResource<ResourceTestCopy>();
            resourceCopy.data.emplace_back("PluginTestUnbind::Startup");
        });
        RegisterSystems<Engine::Scheduler::Update>([](Engine::Core &core) {
            auto &resource = core.GetResource<ResourceTest>();
            resource.data.emplace_back("PluginTestUnbind::Update");
            auto &resourceCopy = core.GetResource<ResourceTestCopy>();
            resourceCopy.data.emplace_back("PluginTestUnbind::Update");
        });
        RegisterSystems<Engine::Scheduler::Shutdown>([](Engine::Core &core) {
            auto &resource = core.GetResource<ResourceTest>();
            resource.data.emplace_back("PluginTestUnbind::Shutdown");
            auto &resourceCopy = core.GetResource<ResourceTestCopy>();
            resourceCopy.data.emplace_back("PluginTestUnbind::Shutdown");
        });
    }
};
} // namespace Example

TEST(Plugin, CasualUse)
{
    Engine::Core core;
    core.AddPlugins<Example::PluginTestB>();
    core.RunSystems();

    auto &resource = core.GetResource<Example::ResourceTest>();
    ASSERT_EQ(resource.data.size(), 2);
    ASSERT_EQ(resource.data[0], "PluginTestA::Bind");
    ASSERT_EQ(resource.data[1], "PluginTestB::Bind");
}

TEST(Plugin, Unbind)
{
    Engine::Core core;

    core.RegisterResource(Example::ResourceTestCopy{});

    core.AddPlugins<Example::PluginTestUnbind>();

    core.RegisterSystem([](Engine::Core &core) {
        static std::uint8_t numberOfRun = 0;
        if (numberOfRun == 0)
        {
            auto pluginOpt = core.GetPlugin<Example::PluginTestUnbind>();
            ASSERT_TRUE(pluginOpt.has_value());

            pluginOpt.value().get()->Unbind();
        }
        else if (numberOfRun == 1)
        {
            auto pluginOpt = core.GetPlugin<Example::PluginTestUnbind>();
            ASSERT_TRUE(pluginOpt.has_value());

            pluginOpt.value().get()->Bind();
        }
        numberOfRun++;
    });

    core.RunSystems();

    {
        ASSERT_FALSE(core.HasResource<Example::ResourceTest>());
        auto &resourceCopy = core.GetResource<Example::ResourceTestCopy>();
        ASSERT_EQ(resourceCopy.data.size(), 3);
        EXPECT_EQ(resourceCopy.data[0], "PluginTestUnbind::Startup");
        EXPECT_EQ(resourceCopy.data[1], "PluginTestUnbind::Update");
        EXPECT_EQ(resourceCopy.data[2], "PluginTestUnbind::Shutdown");
    }

    {
        core.RunSystems();
    }

    core.RunSystems();
    core.RunSystems();
    {
        auto &resource = core.GetResource<Example::ResourceTest>();
        auto &resourceCopy = core.GetResource<Example::ResourceTestCopy>();
        ASSERT_EQ(resourceCopy.data.size(), 6);
        EXPECT_EQ(resourceCopy.data[0], "PluginTestUnbind::Startup");
        EXPECT_EQ(resourceCopy.data[1], "PluginTestUnbind::Update");
        EXPECT_EQ(resourceCopy.data[2], "PluginTestUnbind::Shutdown");
        EXPECT_EQ(resourceCopy.data[3], "PluginTestUnbind::Startup");
        EXPECT_EQ(resourceCopy.data[4], "PluginTestUnbind::Update");
        EXPECT_EQ(resourceCopy.data[5], "PluginTestUnbind::Update");
        ASSERT_EQ(resource.data.size(), 3);
        EXPECT_EQ(resource.data[0], "PluginTestUnbind::Startup");
        EXPECT_EQ(resource.data[1], "PluginTestUnbind::Update");
        EXPECT_EQ(resource.data[2], "PluginTestUnbind::Update");
    }
}
