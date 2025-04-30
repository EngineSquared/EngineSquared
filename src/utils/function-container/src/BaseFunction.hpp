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

    /**
     * @brief External Call function
     * TODO: write better doc
     */
    TReturn Call(TArgs... args) const { return (*this)(args...); }

    /**
     * @brief Pure virtual function to get the unique ID of the function.
     * @return Unique ID of the function.
     */
    virtual unsigned int GetID() const = 0; // TODO: do not use an unsigned int, define a more explicit type
};
} // namespace ES::Utils::FunctionContainer
