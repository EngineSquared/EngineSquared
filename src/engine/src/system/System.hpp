#pragma once

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
    template <typename... TSystem> inline void AddSystems(TSystem... systems) { AddFunctions(systems...); }

    inline const std::list<std::unique_ptr<SystemBase>> &GetSystems() { return GetFunctions(); }

    inline bool DeleteSystem(ES::Utils::FunctionContainer::FunctionID id) { return DeleteFunction(id); }

  private:
    template <typename TCallable> void AddSystem(TCallable callable) { AddFunction(callable); }
};

} // namespace ES::Engine
