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

namespace ES::Engine {
// Forward declaration of Core class.
class Core;

using SystemBase = ES::Utils::FunctionContainer::BaseFunction<void, Core &>;

template <typename TCallable> using System = ES::Utils::FunctionContainer::CallableFunction<TCallable, void, Core &>;

/**
 * @brief Container class for managing multiple systems.
 */
class SystemContainer : public ES::Utils::FunctionContainer::FunctionContainer<void, Core &> {
  public:
    template <typename... TSystem> inline decltype(auto) AddSystems(TSystem... systems)
    {
        return AddFunctions(systems...);
    }

    inline decltype(auto) GetSystems() { return GetFunctions(); }

    inline decltype(auto) DeleteSystem(const ES::Utils::FunctionContainer::FunctionID &id)
    {
        return DeleteFunction(id);
    }

  private:
    template <typename TCallable> void AddSystem(TCallable callable) { AddFunction(callable); }
};

} // namespace ES::Engine
