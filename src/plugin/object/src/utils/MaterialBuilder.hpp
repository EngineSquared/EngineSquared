#pragma once

#include "component/Material.hpp"
#include <map>

namespace Plugin::Object::Utils {
class MaterialBuilder {
  private:
    using Data = std::any;

    struct DummyType {};

    inline const static auto EMPTY_DATA = Data();

  public:
    MaterialBuilder() = default;
    ~MaterialBuilder() = default;
    MaterialBuilder(const MaterialBuilder &other) = delete;
    MaterialBuilder &operator=(const MaterialBuilder &other) = delete;

    template <CCopyable TDataType> void Set(const std::string &key, const TDataType &defaultValue)
    {
        _data[key] = PackData(defaultValue);
    }

    Component::Material Build() const { return Component::Material(_data); }

  private:

    template <CCopyable TDataType> Data PackData(const TDataType &data) const { return std::any(data); }

  private:
    std::map<std::string, Data, std::less<>> _data;
};
} // namespace Plugin::Object::Utils
