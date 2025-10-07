/*
** EPITECH PROJECT, 2025
** EngineSquared
** File description:
** MaterialBuilder
*/

#pragma once

#include "component/Material.hpp"
#include <map>

namespace Plugin::Object::Utils {
class MaterialBuilder {
  private:
    using Data = Component::Material::Data;

  private:
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
    // Can throw if key doesn't exist or if the type is wrong
    template <CCopyable TDataType> TDataType UnpackData(const std::string &key) const
    {
        return std::any_cast<TDataType>(_data.at(key));
    }

    template <CCopyable TDataType> Data PackData(const TDataType &data) const { return std::any(data); }

  private:
    std::map<std::string, Component::Material::Data, std::less<>> _data;
};
} // namespace Plugin::Object::Utils
