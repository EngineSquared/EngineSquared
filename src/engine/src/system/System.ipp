#include "System.hpp"

namespace Engine {

template <typename... TSystem> decltype(auto) SystemContainer::AddSystems(TSystem... systems)
{
    return AddFunctions(systems...);
}

template <typename TCallable> void SystemContainer::AddSystem(TCallable callable) { AddFunction(callable); }
} // namespace Engine
