#include <filesystem>
#include <gtest/gtest.h>
#include <string>

#include "core/Core.hpp"
#include "plugin/PluginRmlui.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/UIContext.hpp"
#include "scheduler/Init.hpp"

void RegisterEventListenerTest(Engine::Core &core)
{
    auto &uiContext = core.GetResource<Rmlui::Resource::UIContext>();

    const std::string testAssetPath = std::filesystem::current_path().string() + "/asset/test.rml";

    ASSERT_NO_THROW(uiContext.LoadDocument(testAssetPath));

    ASSERT_NO_THROW(
        uiContext.RegisterEventListener(*uiContext.GetElementById("test"), "click", [](auto &) { EXPECT_TRUE(true); }));

    ASSERT_TRUE(uiContext.AreInputCallbacksRegistered());
}

void UnregisterEventListenerTest(Engine::Core &core)
{
    auto &uiContext = core.GetResource<Rmlui::Resource::UIContext>();

    const std::string testAssetPath = std::filesystem::current_path().string() + "/asset/test.rml";

    ASSERT_TRUE(uiContext.AreInputCallbacksRegistered());

    ASSERT_TRUE(uiContext.UnregisterEventListener(*uiContext.GetElementById("test"), "click"));

    ASSERT_TRUE(uiContext.AreInputCallbacksRegistered());
}

TEST(RmluiEventHandler, GlobalRun)
{
    Engine::Core core;

    core.AddPlugins<Rmlui::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &coreRef) {
        coreRef.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(
            Graphic::Resource::WindowSystem::None);
    });
}
