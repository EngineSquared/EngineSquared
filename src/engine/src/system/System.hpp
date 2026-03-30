#pragma once

#include <algorithm>
#include <entt/entt.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "CallableFunction.hpp"
#include "FunctionContainer.hpp"
#include "Logger.hpp"

namespace Engine {
class Core;

using SystemBase = FunctionUtils::BaseFunction<void, Core &>;

template <typename TCallable> using System = FunctionUtils::CallableFunction<TCallable, void, Core &>;

/// @class SystemContainer
/// @brief Container class for managing multiple systems.
/// @see FunctionUtils::FunctionContainer
class SystemContainer : public FunctionUtils::FunctionContainer<void, Core &> {
  public:
    /// @brief Adds systems to the container.
    /// @tparam ...TSystem Systems types to add.
    /// @param ...systems Systems to add.
    /// @return a tuple of FunctionIDs for the added systems.
    /// @see FunctionUtils::FunctionID
    /// @todo Put the implementation in the inl file
    template <typename... TSystem> inline decltype(auto) AddSystems(TSystem... systems)
    {
        return AddFunctions(std::forward<TSystem>(systems)...);
    }

    /// @brief Gets the list of systems in the container.
    /// @return Const reference to the vector of unique pointers to SystemBase.
    /// @todo Put the implementation in the cpp file, (remove inline)
    inline decltype(auto) GetSystems() { return GetFunctions(); }

    /// @brief Deletes a system from the container by its FunctionID.
    /// @param id The FunctionID of the system to delete.
    /// @return A unique pointer to the deleted system, or nullptr if not found.
    /// @see FunctionUtils::FunctionID
    /// @todo Put the implementation in the cpp file, (remove inline)
    inline decltype(auto) DeleteSystem(const FunctionUtils::FunctionID &id) { return DeleteFunction(id); }

  private:
    /// @brief Add a system to the container.
    /// @tparam TCallable Type of the callable system.
    /// @param callable The callable system to be added.
    /// @see FunctionUtils::CallableFunction
    template <typename TCallable> void AddSystem(TCallable callable) { AddFunction(callable); }
};

} // namespace Engine
