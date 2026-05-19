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

class RegistryMediator {
  public:
    explicit RegistryMediator(void) = default;
    ~RegistryMediator() = default;

    std::vector<std::string> LoadRegistry(const std::filesystem::path &file);

    const std::filesystem::path &GetLibraryPath(const std::string &name) { return libs.at(name); }

  private:
    std::unordered_map<std::string, std::filesystem::path> libs;
};
} // namespace DynamicPlugin::Resource
