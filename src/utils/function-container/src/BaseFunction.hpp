#pragma once

#include "FunctionID.hpp"
#include <string>

namespace FunctionUtils {
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
     * @param args Arguments to pass to the function.
     * @return Return value of the function.
     * @note This function is used to call the wrapped function when using the operator() is not possible.
     */
    TReturn Call(TArgs... args) const { return (*this)(args...); }

    /**
     * @brief Pure virtual function to get the unique ID of the function.
     * @return Unique ID of the function.
     */
    virtual FunctionID GetID() const = 0;

    /**
     * @brief Pure virtual function to get the name of the function.
     * @return Name of the function.
     */
    virtual std::string GetName() const = 0;
};
} // namespace FunctionUtils
