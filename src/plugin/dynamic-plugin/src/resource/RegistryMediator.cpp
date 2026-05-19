#include "resource/RegistryMediator.hpp"

namespace DynamicPlugin::Resource {

std::vector<std::string> RegistryMediator::LoadRegistry(const std::filesystem::path &file)
{
    std::vector<std::string> newDynamicLibraries{};
    if (!std::filesystem::exists(file))
    {
        Log::Error(fmt::format(
            "DynamicPlugin::Resource::RegistryMediator::TryLoadRegistry: Registry can't be loaded: path not found ({})",
            std::filesystem::absolute(file).c_str()));
        return newDynamicLibraries;
    }
    try
    {
        const auto &items = nlohmann::json::parse(std::ifstream(file));
        for (auto it = items.begin(); it != items.end(); ++it)
        {
            std::string name = it.key();
            std::string path = it.value();
            if (this->libs.contains(name))
            {
                Log::Info(fmt::format("DynamicPlugin::Resource::RegistryMediator::TryLoadRegistry: {} already exists, "
                                      "overriding it (previous: {}, new: {})",
                                      name, this->libs.at(name).string(), path));
            }
            this->libs.emplace(name, path);
            newDynamicLibraries.emplace_back(name);
            Log::Info(fmt::format(
                "DynamicPlugin::Resource::RegistryMediator::TryLoadRegistry: {} have been loaded (path: {})", name,
                path));
        }
    }
    catch (const std::exception &error)
    {
        Log::Error(fmt::format("DynamicPlugin::Resource::RegistryMediator::TryLoadRegistry: {}", error.what()));
    }
    return newDynamicLibraries;
}
} // namespace DynamicPlugin::Resource
