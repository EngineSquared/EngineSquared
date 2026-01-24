#include <gtest/gtest.h>

#include <memory>
#include <sstream>
#include <string>

#include "core/Core.hpp"
#include "plugin/PluginRmlui.hpp"
#include "resource/GraphicSettings.hpp"
#include "resource/UIContext.hpp"
#include "scheduler/Init.hpp"
#include "spdlog/common.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/ostream_sink.h"
#include "spdlog/spdlog.h"

void LoadFontTest(Engine::Core &core)
{
    auto &uiContext = core.GetResource<Rmlui::Resource::UIContext>();

    uiContext.SetFont("asset/LatoLatin-Regular.ttf");
}

void LoadFontFailureTest(Engine::Core &core)
{
    auto &uiContext = core.GetResource<Rmlui::Resource::UIContext>();

    auto previousLogger = spdlog::default_logger();
    std::ostringstream logStream;
    auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(logStream);
    auto testLogger = std::make_shared<spdlog::logger>("rmlui_font_test", sink);
    testLogger->set_level(spdlog::level::warn);
    spdlog::set_default_logger(testLogger);

    uiContext.SetFont("asset/missing.ttf");

    spdlog::set_default_logger(previousLogger);

    const std::string output = logStream.str();
    EXPECT_NE(output.find("Rmlui could not load the font"), std::string::npos);
}

TEST(RmluiFont, GlobalRun)
{
    Engine::Core core;

    core.AddPlugins<Rmlui::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &coreRef) {
        coreRef.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem(LoadFontTest);

    EXPECT_NO_THROW(core.RunSystems());
}

TEST(RmluiFont, FailedLoad)
{
    Engine::Core core;

    core.AddPlugins<Rmlui::Plugin>();

    core.RegisterSystem<RenderingPipeline::Init>([](Engine::Core &coreRef) {
        coreRef.GetResource<Graphic::Resource::GraphicSettings>().SetWindowSystem(Graphic::Resource::WindowSystem::None);
    });

    core.RegisterSystem(LoadFontFailureTest);

    EXPECT_NO_THROW(core.RunSystems());
}