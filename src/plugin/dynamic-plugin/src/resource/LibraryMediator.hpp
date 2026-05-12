#pragma once

#include "Logger.hpp"
#include "fmt/format.h"
#include <filesystem>
#include <fstream>
#include <functional>
#include <nlohmann/json.hpp>
#include <optional>
#include <unordered_map>

namespace DynamicPlugin::Resource {
struct PluginSpecification {
    std::string name;
    std::string path;
};

class LibraryMediator {
  public:
    explicit LibraryMediator(void) = default;
    ~LibraryMediator() = default;

    const std::optional<std::string> TryGetPluginSpecs(std::string plugin_name,
                                                       const std::optional<std::string> &libsName = std::nullopt)
    {
        if (libsName.has_value() && !libs.contains(libsName.value()))
        {
            Log::Error(fmt::format(
                "DynamicPlugin::Resource::LibraryMediator::TryGetPluginPath: {} is not added in registry, can't load {}",
                libsName.value(), plugin_name));
            return std::nullopt;
        }
        if (libsName.has_value())
        {
            if (libs.at(libsName.value()).contains(plugin_name))
            {
                return libs.at(libsName.value()).at(plugin_name);
            }
            else
            {
                Log::Error(
                    fmt::format("DynamicPlugin::Resource::LibraryMediator::TryGetPluginPath: {} is not added in {}",
                                plugin_name, libsName.value()));
                return std::nullopt;
            }
        }
        else
        {
            for (const auto &lib : libs)
            {
                if (lib.second.contains(plugin_name))
                {
                    return lib.second.at(plugin_name);
                }
            }
            Log::Error(
                fmt::format("DynamicPlugin::Resource::LibraryMediator::TryGetPluginPath: {} don't exist in the mediator",
                            plugin_name));
            return std::nullopt;
        }
    }

    bool TryLoadRegistry(const std::string &name, const std::filesystem::path &file)
    {
        if (!std::filesystem::exists(file))
        {
            Log::Error(fmt::format("DynamicPlugin::Resource::LibraryMediator::TryLoadRegistry: {} can't be loaded to "
                                   "registry, path not found ({})",
                                   name, std::filesystem::absolute(file).c_str()));
            return false;
        }
        if (libs.contains(name))
        {
            Log::Warning(fmt::format("DynamicPlugin::Resource::LibraryMediator::TryLoadRegistry: {} is already loaded, "
                                     "it will not be loaded again",
                                     name));
            return false;
        }
        std::unordered_map<std::string, std::string> specs;
        try
        {
            const auto &items = nlohmann::json::parse(std::ifstream(file));
            for (auto it = items.begin(); it != items.end(); ++it)
            {
                std::string name = it.key();
                std::string path = it.value();
                PluginSpecification spec{.name = name, .path = path};
                Log::Info(fmt::format(
                    "DynamicPlugin::Resource::LibraryMediator::TryLoadRegistry: {} have been loaded (path: {})", name,
                    path));
                specs.emplace(name, path);
            }
        }
        catch (const std::exception &error)
        {
            Log::Error(fmt::format("DynamicPlugin::Resource::LibraryMediator::TryLoadRegistry: {}", error.what()));
            return false;
        }
        libs.emplace(name, specs);
        return true;
    }

  private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> libs;
};
} // namespace DynamicPlugin::Resource
