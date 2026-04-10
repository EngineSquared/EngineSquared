#pragma once

#include <algorithm>
#include <entt/entt.hpp>
#include <functional>
#include <iostream>
#include <list>
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
    template <typename... TSystem> decltype(auto) AddSystems(TSystem... systems);

    /// @brief Gets the list of systems in the container.
    /// @return Const reference to the vector of unique pointers to SystemBase.
    const std::list<std::unique_ptr<SystemBase>> &GetSystems();

    /// @brief Deletes a system from the container by its FunctionID.
    /// @param id The FunctionID of the system to delete.
    /// @return A unique pointer to the deleted system, or nullptr if not found.
    /// @see FunctionUtils::FunctionID
    std::unique_ptr<SystemBase> DeleteSystem(const FunctionUtils::FunctionID &id);

  private:
    /// @brief Add a system to the container.
    /// @tparam TCallable Type of the callable system.
    /// @param callable The callable system to be added.
    /// @see FunctionUtils::CallableFunction
    template <typename TCallable> void AddSystem(TCallable callable);
};

} // namespace Engine

  #include "System.ipp"
