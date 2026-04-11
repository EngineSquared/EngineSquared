#include "System.hpp"

namespace Engine {
const std::list<std::unique_ptr<SystemBase>> &SystemContainer::GetSystems() { return GetFunctions(); }

std::unique_ptr<SystemBase> SystemContainer::DeleteSystem(const FunctionUtils::FunctionID &id)
{
    return DeleteFunction(id);
}

} // namespace Engine
