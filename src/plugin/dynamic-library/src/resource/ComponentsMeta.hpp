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
    std::unordered_map<std::string, ComponentMeta> components;

  public:
    inline static const ComponentMeta errorComponentMeta = ComponentMeta{0};

    void AddComponent(const std::string &name, uint32_t id) { components[name] = ComponentMeta{id}; }

    std::optional<std::reference_wrapper<const ComponentMeta>> GetComponent(const std::string &name) const
    {
        auto it = components.find(name);
        if (it != components.end())
        {
            return it->second;
        }
        return std::nullopt;
    }
};
} // namespace DynamicLibrary::Resource
