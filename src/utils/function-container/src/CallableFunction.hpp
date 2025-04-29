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
    explicit CallableFunction(TCallable callable) : _callable(callable) {}

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

protected:
    TCallable _callable;
};
} // namespace ES::Utils::FunctionContainer