#pragma once

#include "BaseFunction.hpp"

namespace ES::Utils::FunctionContainer {
/**
 * @brief Abstract class that holds a callable object, to be later stored in a container.
 */
template <typename TCallable, typename TReturn, typename... TArgs>
class CallableFunction : public BaseFunction<TReturn, TArgs...> {
  public:
    /**
     * @brief Constructor for CallableFunction.
     * @param callable The callable object to be stored.
     */
    explicit CallableFunction(TCallable callable) : _callable(callable) { _id = GetCallableID(_callable); }

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
     * @brief Get the ID of the callable object.
     * @param callable The callable object.
     * @tparam TCallable The type of the callable object.
     * @return The ID of the callable object.
     */
    static FunctionID GetCallableID(TCallable callable)
    {
        if constexpr (std::is_class_v<TCallable>)
        {
            return typeid(callable).hash_code();
        }
        else
        {
            return std::hash<TCallable>{}(callable);
        }
    }

  private:
    [[no_unique_address]] TCallable _callable;
    FunctionID _id = 0; ///< Unique ID for the function.
};
} // namespace ES::Utils::FunctionContainer
