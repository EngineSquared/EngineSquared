#pragma once

#include "Logger.hpp"
#include <array>
#include <dylib.hpp>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace DynamicLibrary::Resource {
class Handle {
  public:
    Handle(const std::filesystem::path &path) : lib(path.string(), dylib::decorations::none()) {}
    ~Handle() = default;

    template <typename TFunctionType> auto GetFunction(const std::string &functionName)
    {
        const std::array<std::string, 3> candidates = {
            functionName,
            "_" + functionName,
            functionName + "@4",
        };

        for (const auto &candidate : candidates)
        {
            try
            {
                return lib.get_function<TFunctionType>(candidate);
            }
            catch (const std::exception &)
            {
                continue;
            }
        }

        throw std::runtime_error("Failed to resolve symbol '" + functionName + "' on Windows");
    }

  private:
    dylib::library lib;
};
} // namespace DynamicLibrary::Resource
