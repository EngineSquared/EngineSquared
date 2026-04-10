#include "System.hpp"

namespace Engine {
const std::list<std::unique_ptr<SystemBase>> &SystemContainer::GetSystems() { return GetFunctions(); }

std::unique_ptr<SystemBase> SystemContainer::DeleteSystem(const FunctionUtils::FunctionID &id)
{
    return DeleteFunction(id);
}

template <typename... TSystem> decltype(auto) SystemContainer::AddSystems(TSystem... systems)
{
    return AddFunctions(systems...);
}

template <typename TCallable> void SystemContainer::AddSystem(TCallable callable) { AddFunction(callable); }
} // namespace Engine
