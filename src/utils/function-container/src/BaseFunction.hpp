#pragma once

namespace ES::Utils::FunctionContainer {
/**
 * @brief Base class for all functions contained in a FunctionContainer.
 */
template <typename TReturn, typename... TArgs> class BaseFunction {
  public:
    virtual ~BaseFunction() = default;

    /**
     * @brief Pure virtual function call operator to be implemented by derived functions.
     * @param args Arguments to pass to the function.
     * @return Return value of the function.
     */
    virtual TReturn operator()(TArgs... args) const = 0;
};
} // namespace ES::Utils::FunctionContainer
