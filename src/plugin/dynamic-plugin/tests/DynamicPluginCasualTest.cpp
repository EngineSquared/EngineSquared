#include <gtest/gtest.h>

#include "DynamicPlugin.hpp"
#include "scheduler/Startup.hpp"
#include <dylib.hpp>

dylib::library LoadDynamicLibrary(const std::filesystem::path &path)
{
    return dylib::library{path.string(), dylib::decorations::none()};
}

TEST(DynamicPlugin, SmokeTest)
{
    Engine::Core core;

    Log::SetLevel(Log::Level::trace);

    core.AddPlugins<DynamicPlugin::Plugin>();

    core.RegisterSystem<Engine::Scheduler::Update>([](Engine::Core &core) {
        auto &regMed = core.GetResource<DynamicPlugin::Resource::RegistryMediator>();
        auto &dynLibContainer = core.GetResource<DynamicPlugin::Resource::DynamicLibraryContainer>();
        for (const auto &dynLibName : regMed.LoadRegistry("./tests/dynamicPlugins/casual/libs.json"))
        {
            const auto &dynLibPath = regMed.GetLibraryPath(dynLibName);
            dylib::library dynlib = LoadDynamicLibrary(dynLibPath);
            entt::hashed_string libIdName{dynLibName.c_str(), dynLibName.size()};
            dynLibContainer.Add(libIdName, std::move(dynlib));
            std::unique_ptr<Engine::APlugin> newPlugin{
                dynLibContainer.Get(libIdName).get_function<Engine::APlugin *(Engine::Core *)>("CreatePlugin")(&core)};
            core.AddPlugin(dynLibName, std::move(newPlugin));
        }
    });

    core.RunSystems();
}
