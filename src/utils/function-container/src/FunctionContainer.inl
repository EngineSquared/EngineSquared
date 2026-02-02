#include "CallableFunction.hpp"
#include "FunctionContainer.hpp"
#include "Logger.hpp"

template <typename TReturn, typename... TArgs>
template <typename TCallable>
FunctionUtils::FunctionID FunctionUtils::FunctionContainer<TReturn, TArgs...>::AddFunction(TCallable callable)
{
    FunctionUtils::FunctionID id;
    std::string name;

    if constexpr (is_derived_from_function_type<TCallable>::value)
    {
        id = callable.GetID();
        name = callable.GetName();
    }
    else
    {
        id = CallableFunction<TCallable, TReturn, TArgs...>::GetCallableID(callable);
        name = CallableFunction<TCallable, TReturn, TArgs...>::GetCallableName(callable);
    }

    if (_idToIterator.contains(id))
    {
        Log::Warn(fmt::format("Function already exists: {}", name)); // TODO: be able to change container thing name
        return id;
    }

    auto function = std::make_unique<CallableFunction<TCallable, TReturn, TArgs...>>(callable);
    _orderedFunctions.push_back(std::move(function));

    auto it = std::prev(_orderedFunctions.end());
    _idToIterator[id] = it;

    return id;
}

template <typename TReturn, typename... TArgs>
FunctionUtils::FunctionID FunctionUtils::FunctionContainer<TReturn, TArgs...>::AddFunction(
    std::unique_ptr<BaseFunction<TReturn, TArgs...>> &&function)
{
    FunctionUtils::FunctionID id = function->GetID();

    if (_idToIterator.contains(id))
    {
        Log::Warn("Function already exists: " + function->GetName()); // TODO: be able to change container thing name
        return id;
    }

    _orderedFunctions.push_back(std::move(function));

    auto it = std::prev(_orderedFunctions.end());
    _idToIterator[id] = it;

    return id;
}

template <typename TReturn, typename... TArgs>
std::unique_ptr<typename FunctionUtils::FunctionContainer<TReturn, TArgs...>::FunctionType>
FunctionUtils::FunctionContainer<TReturn, TArgs...>::DeleteFunction(FunctionUtils::FunctionID id)
{
    auto mapIt = _idToIterator.find(id);
    if (mapIt == _idToIterator.end())
    {
        Log::Warn("Function not found");
        return nullptr;
    }

    auto listIt = mapIt->second;
    auto func = std::move(*listIt);

    _orderedFunctions.erase(listIt);
    _idToIterator.erase(mapIt);

    return func;
}
