#include <gtest/gtest.h>

#include <filesystem>
#include <string>

#include "core/Core.hpp"
#include "exception/ReadRmlDocumentError.hpp"
#include "plugin/PluginRmlui.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/UIContext.hpp"
#include "scheduler/Init.hpp"

void LoadOverlayDocumentTest(Engine::Core &core)
{
    auto &uiContext = core.GetResource<Rmlui::Resource::UIContext>();

    const std::string testAssetPath = std::filesystem::current_path().string() + "/asset/test.rml";

    ASSERT_NO_THROW(uiContext.LoadOverlayDocument(testAssetPath));
}

void LoadOverlayDocumentFailureTest(Engine::Core &core)
{
    auto &uiContext = core.GetResource<Rmlui::Resource::UIContext>();

    const std::string testAssetPath = std::filesystem::current_path().string() + "/asset/missing.rml";

    EXPECT_THROW(uiContext.LoadOverlayDocument(testAssetPath), Rmlui::Exception::ReadRmlDocumentError);
}

void UnloadOverlayDocumentTest(Engine::Core &core)
{
    auto &uiContext = core.GetResource<Rmlui::Resource::UIContext>();

    const std::string testAssetPath = std::filesystem::current_path().string() + "/asset/test.rml";

    ASSERT_TRUE(uiContext.LoadOverlayDocument(testAssetPath));

    ASSERT_NO_THROW(uiContext.UnloadOverlayDocument(testAssetPath));
}

void UnloadOverlayDocumentFailureTest(Engine::Core &core)
{
    auto &uiContext = core.GetResource<Rmlui::Resource::UIContext>();

    const std::string testAssetPath = std::filesystem::current_path().string() + "/asset/missing.rml";

    EXPECT_THROW(uiContext.UnloadOverlayDocument(testAssetPath), Rmlui::Exception::ReadRmlDocumentError);
}

void LoadDocumentTest(Engine::Core &core)
{
    auto &uiContext = core.GetResource<Rmlui::Resource::UIContext>();

    const std::string testAssetPath = std::filesystem::current_path().string() + "/asset/test.rml";

    ASSERT_NO_THROW(uiContext.LoadDocument(testAssetPath));

    EXPECT_NE(uiContext.GetDocument(), nullptr);
    EXPECT_NE(uiContext.GetElementById("test"), nullptr);
}

void LoadDocumentFailureTest(Engine::Core &core)
{
    auto &uiContext = core.GetResource<Rmlui::Resource::UIContext>();

    const std::string testAssetPath = std::filesystem::current_path().string() + "/asset/missing.rml";

    EXPECT_THROW(uiContext.LoadDocument(testAssetPath), Rmlui::Exception::ReadRmlDocumentError);
}

TEST(RmluiDocument, GlobalRun)
{
    Engine::Core core;

    core.AddPlugins<Rmlui::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &coreRef) {
        coreRef.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem(LoadDocumentTest);

    EXPECT_NO_THROW(core.RunSystems());
}

TEST(RmluiDocument, FailedLoad)
{
    Engine::Core core;

    core.AddPlugins<Rmlui::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &coreRef) {
        coreRef.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem(LoadDocumentFailureTest);

    EXPECT_NO_THROW(core.RunSystems());
}

TEST(RmluiDocument, LoadOverlayDocument)
{
    Engine::Core core;

    core.AddPlugins<Rmlui::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &coreRef) {
        coreRef.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem(LoadOverlayDocumentTest);
}

TEST(RmluiDocument, FailedLoadOverlayDocument)
{
    Engine::Core core;

    core.AddPlugins<Rmlui::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &coreRef) {
        coreRef.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem(LoadOverlayDocumentFailureTest);
}

TEST(RmluiDocument, UnloadOverlayDocument)
{
    Engine::Core core;

    core.AddPlugins<Rmlui::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &coreRef) {
        coreRef.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem(UnloadOverlayDocumentTest);
}
