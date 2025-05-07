#pragma once

#include "BaseFunction.hpp"

#include <list>
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
    template <typename TCallable> FunctionID AddFunction(TCallable callable);

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
     * @return Const reference to the vector of unique pointers to BaseFunction.
     */
    inline const std::list<std::unique_ptr<BaseFunction<TReturn, TArgs...>>> &GetFunctions()
    {
        return _orderedFunctions;
    }

    /**
     * @brief Returns true if the container is empty.
     * @return True if the container is empty, false otherwise.
     */
    inline bool IsEmpty() const { return _orderedFunctions.empty(); }

    /**
     * @brief Returns the number of functions in the container.
     * @return The number of functions in the container.
     */
    inline std::size_t Size() const { return _orderedFunctions.size(); }

    /**
     * @brief Deletes a function from the container.
     * @param id The ID of the function to be deleted.
     * @return True if the function was deleted, false otherwise.
     */
    bool DeleteFunction(FunctionID id);

  private:
    std::unordered_map<FunctionID, std::size_t> _idToIndex; ///< Map to store unique ids for each function.
    std::list<std::unique_ptr<BaseFunction<TReturn, TArgs...>>>
        _orderedFunctions; ///< Vector to store functions in order.
};
} // namespace ES::Utils::FunctionContainer

#include "FunctionContainer.inl"
