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

namespace Plugin::Object::Component {
class Material {
  public:
    using Data = std::any;

  private:
    struct DummyType {};
    inline const static auto EMPTY_DATA = Data();

  public:
    Material(std::map<std::string, Data> defaultData)
    {
        for (const auto &[name, value] : defaultData)
        {
            _data[name] = value;
        }
    }
    ~Material() = default;
    Material(const Material &other) = default;
    Material &operator=(const Material &other) = default;

    template <CCopyable TDataType> TDataType Get(const std::string &key) const
    {
        std::optional<std::string> err = ContainsWithError<TDataType>(key);
        if (err.has_value())
            throw std::runtime_error("Material::Get: " + err.value());
        return UnpackData<TDataType>(key);
    }

    template <CCopyable TDataType> void Set(const std::string &key, const TDataType &value)
    {
        std::optional<std::string> err = ContainsWithError<TDataType>(key, false);
        if (err.has_value())
            throw std::runtime_error("Material::Set: " + err.value());
        _data[key] = PackData(value);
    }

    template <CCopyable TDataType = DummyType> bool Contains(const std::string &key, bool checkValueInside = true) const
    {
        auto it = _data.find(key);
        if (it == _data.end())
            return false;
        if constexpr (!std::is_same_v<TDataType, DummyType>)
        {
          if (it->second.type() != typeid(TDataType))
              return false;
        }
        if (checkValueInside && !it->second.has_value())
            return false;
        return true;
    }

    template <CCopyable TDataType = DummyType>
    std::optional<std::string> ContainsWithError(const std::string &key, bool checkValueInside = true) const
    {
        auto it = _data.find(key);
        if (it == _data.end())
            return "key '" + key + "' doesn't exist";
        if (it->second.type() != typeid(TDataType))
            return "key '" + key + "' type mismatch";
        if (checkValueInside && !it->second.has_value())
            return "key '" + key + "' has no value";
        return std::nullopt;
    }

  private:
    // Can throw if key doesn't exist or if the type is wrong
    template <CCopyable TDataType> TDataType UnpackData(const std::string &key) const
    {
        return std::any_cast<TDataType>(_data.at(key));
    }

    template <CCopyable TDataType> Data PackData(const TDataType &data) const { return std::any(data); }

  private:
    std::map<std::string, Data> _data;
};
} // namespace Plugin::Object::Component
