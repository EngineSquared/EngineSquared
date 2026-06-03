#include "CallableFunction.hpp"
#include "FunctionContainer.hpp"
#include "Logger.hpp"

template <typename TReturn, typename... TArgs>
template <typename TCallable>
FunctionUtils::FunctionID FunctionUtils::FunctionContainer<TReturn, TArgs...>::AddFunction(TCallable callable)
{
    FunctionUtils::FunctionID id = CallableFunction<TCallable, TReturn, TArgs...>::GetCallableID(callable);
    std::string name = CallableFunction<TCallable, TReturn, TArgs...>::GetCallableName(callable);

    if (_idToIterator.contains(id))
    {
        Log::Warning(fmt::format("Function already exists: {}", name)); // TODO: be able to change container thing name
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
    FunctionUtils::FunctionID id =
        CallableFunction<const std::unique_ptr<BaseFunction<TReturn, TArgs...>> &, TReturn, TArgs...>::GetCallableID(
            function);

    if (_idToIterator.contains(id))
    {
        Log::Warning(
            "Function already exists: " +
            CallableFunction<const std::unique_ptr<BaseFunction<TReturn, TArgs...>> &, TReturn,
                             TArgs...>::GetCallableName(function)); // TODO: be able to change container thing name
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
        Log::Warning("Function not found");
        return nullptr;
    }

    auto listIt = mapIt->second;
    auto func = std::move(*listIt);

    _orderedFunctions.erase(listIt);
    _idToIterator.erase(mapIt);

    return func;
}
