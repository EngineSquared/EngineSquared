#pragma once

#include "BaseFunction.hpp"
#include "Demangle.hpp"
#include <concepts>
#include <string>
#include <typeinfo>

template <typename TIdGettable>
concept CIdGettable = requires(TIdGettable idGettable) {
    { idGettable->GetID() } -> std::convertible_to<FunctionUtils::FunctionID>;
};

template <typename TNameGettable>
concept CNameGettable = requires(TNameGettable nameGettable) {
    { nameGettable->GetName() } -> std::convertible_to<std::string>;
};

namespace FunctionUtils {
/**
 * @brief Abstract class that holds a callable object, to be later stored in a container.
 */
template <typename TCallable, typename TReturn, typename... TArgs>
class CallableFunction : public BaseFunction<TReturn, TArgs...> {
  public:
    using FunctionType = BaseFunction<TReturn, TArgs...>;

    /**
     * @brief Type trait to check if a type is derived from BaseFunction.
     * @tparam T The type to check.
     * @tparam TReturn The return type of the function.
     * @tparam TArgs The argument types of the function.
     */
    template <typename T> struct is_derived_from_function_type : std::is_base_of<FunctionType, std::decay_t<T>> {};
    /**
     * @brief Constructor for CallableFunction.
     * @param callable The callable object to be stored.
     */
    explicit CallableFunction(TCallable callable) : _callable(callable)
    {
        _id = GetCallableID(_callable);
        _name = GetCallableName(_callable);
    }

    /**
     * @brief Destructor for CallableFunction.
     */
    ~CallableFunction() override = default;

    /**
     * @brief Calls the stored callable object with the provided arguments.
     * @param args Arguments to pass to the callable object.
     * @return Return value of the callable object.
     */
    TReturn operator()(TArgs... args) const override { return _callable(args...); }

    /**
     * @brief Returns the unique ID of the callable object.
     * @return Unique ID of the callable object.
     */
    FunctionID GetID() const override { return _id; }

    /**
     * @brief Returns the name of the callable object.
     * @return Name of the callable object.
     */
    std::string GetName() const override { return _name; }

    /**
     * @brief Get the ID of the callable object.
     * @param callable The callable object.
     * @tparam TCallable The type of the callable object.
     * @return The ID of the callable object.
     */
    static FunctionID GetCallableID(TCallable callable)
    {
        if constexpr (is_derived_from_function_type<TCallable>::value)
        {
            return callable.GetID();
        }
        else if constexpr (CIdGettable<TCallable>)
        {
            return callable->GetID();
        }
        else if constexpr (std::is_class_v<TCallable>)
        {
            return typeid(callable).hash_code();
        }
        else
        {
            return std::hash<TCallable>{}(callable);
        }
    }

    static std::string GetCallableName(TCallable callable)
    {
        if constexpr (is_derived_from_function_type<TCallable>::value)
        {
            return callable.GetName();
        }
        else if constexpr (CNameGettable<TCallable>)
        {
            return callable->GetName();
        }
        else if constexpr (std::is_class_v<TCallable>)
        {
            return FunctionUtils::DemangleTypeName(typeid(callable));
        }
        else
        {
            return std::to_string(GetCallableID(callable));
        }
    }

  private:
    [[no_unique_address]] TCallable _callable;
    FunctionID _id = 0; ///< Unique ID for the function.
    std::string _name;  ///< Name of the function.
};
} // namespace FunctionUtils
