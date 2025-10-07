/*
** EPITECH PROJECT, 2025
** EngineSquared
** File description:
** Material
*/

#pragma once

#include <any>
#include <concepts>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>

template <typename T>
concept CCopyable = std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>;

namespace Plugin::Object::Resource {
class Material {
  public:
    using Data = std::any;

  private:
    struct DummyType {};

  public:
    Material() = default;
    ~Material() = default;
    Material(const Material &other) = default;
    Material &operator=(const Material &other) = default;

    template <CCopyable TDataType> void Set(const std::string &key, const TDataType &data)
    {
        _data[key] = PackData<TDataType>(data);
    }

    template <CCopyable TDataType>
    TDataType Get(const std::string &key) const
    {
        return UnpackData<TDataType>(key);
    }

    template <CCopyable TDataType = DummyType>
    bool Contains(const std::string &key) const
    {
        if constexpr (std::is_same_v<TDataType, DummyType>) {
            return _data.contains(key);
        } else {
            auto it = _data.find(key);
            if (it == _data.end())
                return false;
            return it->second.type() == typeid(TDataType);
        }
    }

  private:
    // Can throw if key doesn't exist or if the type is wrong
    template <CCopyable TDataType>
    TDataType UnpackData(const std::string &key) const
    {
        return std::any_cast<TDataType>(_data.at(key));
    }

    template <CCopyable TDataType>
    Data PackData(const TDataType &data) const
    {
        return std::any(data);
    }

  private:
    std::map<std::string, Data> _data;
};
}
