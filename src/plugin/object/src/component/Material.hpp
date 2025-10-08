#pragma once

#include <any>
#include <concepts>
#include <map>
#include <stdexcept>
#include <string>
#include <type_traits>

#include "exception/MaterialError.hpp"

template <typename T>
concept CCopyable = std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T>;

namespace Plugin::Object::Component {
class Material {
  private:
    using Data = std::any;

    struct DummyType {};

    inline const static auto EMPTY_DATA = Data();

  public:
    explicit Material(const std::map<std::string, Data, std::less<>> &defaultData)
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
        if (!Contains<TDataType>(key))
            throw MaterialError("Material::Get: Key '" + key + "' is either dont exist or has a different type");
        return UnpackData<TDataType>(key);
    }

    template <CCopyable TDataType> void Set(const std::string &key, const TDataType &value)
    {
        if (!Contains<TDataType>(key))
            throw MaterialError("Material::Set: Key '" + key + "' is either dont exist or has a different type");
        _data[key] = PackData(value);
    }

    template <CCopyable TDataType = DummyType> bool Contains(const std::string &key) const
    {
        auto it = _data.find(key);
        if (it == _data.end())
            return false;
        if constexpr (!std::is_same_v<TDataType, DummyType>)
        {
            if (it->second.type() != typeid(TDataType))
                return false;
        }
        return true;
    }

  private:
    // Can throw if key doesn't exist or if the type is wrong
    template <CCopyable TDataType> TDataType UnpackData(const std::string &key) const
    {
        return std::any_cast<TDataType>(_data.at(key));
    }

    template <CCopyable TDataType> Data PackData(const TDataType &data) const { return std::any(data); }

  private:
    std::map<std::string, Data, std::less<>> _data;
};
} // namespace Plugin::Object::Component
