#pragma once

#include "BaseFunction.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace ES::Utils::FunctionContainer {
/**
 * @brief Container for functions, allowing for dynamic storage and invocation.
 **/
template <typename TReturn, typename... TArgs> class FunctionContainer {
  public:
    /**
     * @brief Default constructor for FunctionContainer.
     */
    FunctionContainer() = default;

    /**
     * @brief Default destructor for FunctionContainer.
     */
    ~FunctionContainer() = default;

    /**
     * @brief Adds a function to the container.
     * @tparam TCallable Type of the callable function.
     * @param callable The callable function to be added.
     */
    template <typename TCallable>
    FunctionID AddFunction(TCallable callable); // TODO: return the ID when a callable is created

    /**
     * @brief Adds a wrapped function to the container.
     * @param function A wrapped function to be added.
     * @note A wrapped function is a unique pointer to a class derived from BaseFunction.
     */
    FunctionID AddFunction(std::unique_ptr<BaseFunction<TReturn, TArgs...>> &&function);

    /**
     * @brief Adds multiple functions to the container.
     * @tparam TFunctions Variadic template parameter for function types.
     * @param functions The functions to be added.
     */
    template <typename... TFunctions> void AddFunctions(TFunctions... functions) { (AddFunction(functions), ...); }

    /**
     * @brief Gets the list of functions in the container.
     * @return Reference to the vector of unique pointers to BaseFunction.
     */
    inline std::list<std::unique_ptr<BaseFunction<TReturn, TArgs...>>> &GetFunctions() { return _orderedFunctions; }

  protected:
    std::unordered_map<FunctionID, std::size_t> _idToIndex; ///< Map to store unique ids for each function.
    std::list<std::unique_ptr<BaseFunction<TReturn, TArgs...>>>
        _orderedFunctions; ///< Vector to store functions in order.
};
} // namespace ES::Utils::FunctionContainer

#include "FunctionContainer.inl"
