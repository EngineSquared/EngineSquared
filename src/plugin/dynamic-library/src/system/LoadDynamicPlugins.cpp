#include "system/LoadDynamicPlugins.hpp"
#include "resource/DynamicPlugin.hpp"
#include "scheduler/Startup.hpp"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

namespace DynamicLibrary::System {
static std::optional<std::unordered_map<std::string, std::filesystem::path>>
GetDynamicPlugins(std::filesystem::path pluginsFile)
{
    std::unordered_map<std::string, std::filesystem::path> plugins;
    if (!std::filesystem::exists(pluginsFile))
    {
        std::string pluginsFileStr = pluginsFile.string();
        Log::Error(fmt::format("Plugins file not found at path: {}", pluginsFileStr));
        return std::nullopt;
    }
    std::ifstream file(pluginsFile);
    nlohmann::json data = nlohmann::json::parse(file);
    for (const auto &pair : data.items())
    {
        try
        {
            plugins[pair.key()] = pair.value().get<std::string>();
        }
        catch (const std::exception &e)
        {
            Log::Error(fmt::format("Failed to parse plugin path for '{}': {}", pair.key(), e.what()));
        }
    }
    return plugins;
}

void LoadDynamicPlugins(Engine::Core &core)
{
    const std::filesystem::path pluginsFile = std::filesystem::current_path() / "libs.json";
    if (!std::filesystem::exists(pluginsFile))
    {
        Log::Info(fmt::format("No plugins file found at path: {}", pluginsFile.string()));
        return;
    }
    auto libs = GetDynamicPlugins(pluginsFile);
    if (!libs)
    {
        Log::Error("Failed to load dynamic plugins");
        return;
    }
    for (const auto &[name, path] : *libs)
    {
        const auto &name1 = name;
        const auto &path1 = path;
        core.AddPlugin(name1, std::make_unique<Resource::DynamicPlugin>(core, path1));
    }
}
} // namespace DynamicLibrary::System
