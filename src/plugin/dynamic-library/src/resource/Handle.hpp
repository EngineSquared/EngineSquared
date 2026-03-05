#pragma once

#include "Logger.hpp"
#include <dlfcn.h>
#include <filesystem>

namespace DynamicLibrary::Resource {
class Handle {
  public:
    Handle(std::filesystem::path path) : handle(dlopen(path.c_str(), RTLD_LAZY))
    {
        if (!handle)
        {
            Log::Error(fmt::format("Failed to load library at path: {}, error: {}", path.string(), dlerror()));
            throw std::runtime_error("Failed to load library");
        }
    }
    ~Handle()
    {
        if (handle)
        {
            dlclose(handle);
        }
    }

    void *GetSymbol(const std::string &symbolName) { return dlsym(handle, symbolName.c_str()); }

  private:
    void *handle = nullptr;
};
} // namespace DynamicLibrary::Resource
