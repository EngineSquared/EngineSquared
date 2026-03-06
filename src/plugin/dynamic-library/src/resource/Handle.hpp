#pragma once

#include "Logger.hpp"
#include <dylib.hpp>
#include <filesystem>

namespace DynamicLibrary::Resource {
class Handle {
  public:
    Handle(const std::string &path) : lib(path, dylib::decorations::none()) {}
    ~Handle() = default;

    template <typename TFunctionType> auto GetFunction(const std::string &functionName)
    {
        return lib.get_function<TFunctionType>(functionName);
    }

  private:
    dylib::library lib;
};
} // namespace DynamicLibrary::Resource
