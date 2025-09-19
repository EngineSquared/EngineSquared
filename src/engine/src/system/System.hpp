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
// Forward declaration of Core class.
class Core;

using SystemBase = FunctionUtils::BaseFunction<void, Core &>;

template <typename TCallable> using System = FunctionUtils::CallableFunction<TCallable, void, Core &>;

/**
 * @brief Container class for managing multiple systems.
 */
class SystemContainer : public FunctionUtils::FunctionContainer<void, Core &> {
  public:
    template <typename... TSystem> inline decltype(auto) AddSystems(TSystem... systems)
    {
        return AddFunctions(systems...);
    }

    inline decltype(auto) GetSystems() { return GetFunctions(); }

    inline decltype(auto) DeleteSystem(const FunctionUtils::FunctionID &id)
    {
        return DeleteFunction(id);
    }

  private:
    template <typename TCallable> void AddSystem(TCallable callable) { AddFunction(callable); }
};

} // namespace Engine
