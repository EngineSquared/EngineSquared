#include "WrappedSystem.hpp"

namespace Engine {
template <typename TSystem, typename TErrorCallback>
WrappedSystem<TSystem, TErrorCallback>::WrappedSystem(TSystem system, TErrorCallback errorCallback)
    : _system(system), _errorCallback(errorCallback)
{
    _id = GetCallableID(_system);
    _name = GetCallableName(_system);
}

template <typename TSystem, typename TErrorCallback>
void WrappedSystem<TSystem, TErrorCallback>::operator()(Engine::Core &core) const
{
    try
    {
        return _system(core);
    }
    catch (const std::exception &) // NOSONAR
    {
        _errorCallback(core);
        throw;
    }
}

template <typename TSystem, typename TErrorCallback>
FunctionUtils::FunctionID WrappedSystem<TSystem, TErrorCallback>::GetID() const
{
    return _id;
}

template <typename TSystem, typename TErrorCallback> std::string WrappedSystem<TSystem, TErrorCallback>::GetName() const
{
    return _name;
}

template <typename TSystem, typename TErrorCallback>
FunctionUtils::FunctionID WrappedSystem<TSystem, TErrorCallback>::GetCallableID(const TSystem &callable)
{
    if constexpr (std::is_class_v<TSystem>)
    {
        return typeid(callable).hash_code();
    }
    else
    {
        return std::hash<TSystem>{}(callable);
    }
}

template <typename TSystem, typename TErrorCallback>
std::string WrappedSystem<TSystem, TErrorCallback>::GetCallableName(const TSystem &callable)
{
    if constexpr (std::is_class_v<TSystem>)
    {
        return FunctionUtils::DemangleTypeName(typeid(callable));
    }
    else
    {
        return std::to_string(GetCallableID(callable));
    }
}
} // namespace Engine
