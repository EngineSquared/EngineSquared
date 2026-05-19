#pragma once

#include "Logger.hpp"
#include "dylib.hpp"
#include "fmt/format.h"
#include "resource/ResourceManager.hpp"
#include <filesystem>
#include <vector>

namespace DynamicPlugin::Resource {
class DynamicLibraryContainer : public Object::Resource::ResourceManager<dylib::library> {
  public:
    DynamicLibraryContainer(void) = default;
    ~DynamicLibraryContainer() = default;

    DynamicLibraryContainer(const DynamicLibraryContainer &) = delete;
    DynamicLibraryContainer &operator=(const DynamicLibraryContainer &) = delete;

    DynamicLibraryContainer(DynamicLibraryContainer &&) noexcept = default;
    DynamicLibraryContainer &operator=(DynamicLibraryContainer &&) noexcept = default;
};
} // namespace DynamicPlugin::Resource
