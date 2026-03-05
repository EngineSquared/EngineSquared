#include "system/LoadDynamicPlugins.hpp"
#include "resource/DynamicPlugin.hpp"
#include "scheduler/Startup.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>

namespace DynamicLibrary::System {
static std::unordered_map<std::string, std::filesystem::path> GetDynamicPlugins(std::filesystem::path pluginsFile)
{
    std::unordered_map<std::string, std::filesystem::path> plugins;
    if (!std::filesystem::exists(pluginsFile))
    {
        std::string pluginsFileStr = pluginsFile.string();
        Log::Error(fmt::format("Plugins file not found at path: {}", pluginsFileStr));
        return plugins;
    }
    std::ifstream file(pluginsFile);
    nlohmann::json data = nlohmann::json::parse(file);
    for (const auto &pair : data.items())
    {
        plugins[pair.key()] = pair.value().get<std::string>();
    }
    return plugins;
}

void LoadDynamicPlugins(Engine::Core &core)
{
    auto libs = GetDynamicPlugins(std::filesystem::current_path() / "libs.json");
    for (const auto &[name, path] : libs)
    {
        core.AddPlugin(name, std::make_unique<Resource::DynamicPlugin>(core, path));
    }
}
} // namespace DynamicLibrary::System
