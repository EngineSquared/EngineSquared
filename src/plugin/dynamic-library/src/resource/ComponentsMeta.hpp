#pragma once

#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

namespace DynamicLibrary::Resource {
class ComponentMeta {
  public:
    uint32_t id;
};

class ComponentsMeta {
  private:
    /// @brief A transparent hash function for strings, allowing the use of std::string_view and const char* as keys in
    ///     the unordered_map without the need for explicit conversions.
    struct TransparentHash {
        using is_transparent = void;
        std::size_t operator()(const std::string &str) const noexcept { return std::hash<std::string>{}(str); }
        std::size_t operator()(std::string_view str) const noexcept { return std::hash<std::string_view>{}(str); }
        std::size_t operator()(const char *str) const noexcept { return std::hash<std::string_view>{}(str); }
    };
    std::unordered_map<std::string, ComponentMeta, TransparentHash, std::equal_to<>> components;

  public:
    inline static const ComponentMeta errorComponentMeta = ComponentMeta{0};

    void AddComponent(const std::string &name, uint32_t id) { components[name] = ComponentMeta{id}; }

    std::optional<std::reference_wrapper<const ComponentMeta>> GetComponent(const std::string &name) const
    {
        if (auto it = components.find(name); it != components.end())
        {
            return it->second;
        }
        return std::nullopt;
    }
};
} // namespace DynamicLibrary::Resource
