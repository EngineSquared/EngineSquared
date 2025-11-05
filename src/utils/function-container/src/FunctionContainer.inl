#include "CallableFunction.hpp"
#include "FunctionContainer.hpp"
#include "Logger.hpp"

template <typename TReturn, typename... TArgs>
template <typename TCallable>
FunctionUtils::FunctionID FunctionUtils::FunctionContainer<TReturn, TArgs...>::AddFunction(TCallable callable)
{
    FunctionUtils::FunctionID id;

    if constexpr (is_derived_from_function_type<TCallable>::value)
    {
        id = callable.GetID();
    }
    else
    {
        id = CallableFunction<TCallable, TReturn, TArgs...>::GetCallableID(callable);
    }

    if (_idToIndex.contains(id))
    {
        Log::Warn("Function already exists"); // TODO: be able to change container thing name
        return id;
    }

    std::size_t index = _orderedFunctions.size();
    auto function = std::make_unique<CallableFunction<TCallable, TReturn, TArgs...>>(callable);
    _idToIndex[id] = index;
    _orderedFunctions.push_back(std::move(function));
    return id;
}

template <typename TReturn, typename... TArgs>
FunctionUtils::FunctionID FunctionUtils::FunctionContainer<TReturn, TArgs...>::AddFunction(
    std::unique_ptr<BaseFunction<TReturn, TArgs...>> &&function)
{
    FunctionUtils::FunctionID id = function->GetID();

    if (_idToIndex.contains(id))
    {
        Log::Warn("Function already exists"); // TODO: be able to change container thing name
        return id;
    }

    std::size_t index = _orderedFunctions.size();
    _orderedFunctions.push_back(std::move(function));
    _idToIndex[id] = index;
    return id;
}

template <typename TReturn, typename... TArgs>
std::unique_ptr<typename FunctionUtils::FunctionContainer<TReturn, TArgs...>::FunctionType>
FunctionUtils::FunctionContainer<TReturn, TArgs...>::DeleteFunction(FunctionUtils::FunctionID id)
{
    auto it = _idToIndex.find(id);
    if (it == _idToIndex.end())
    {
        Log::Warn("Function not found");
        return nullptr;
    }

    std::size_t index = it->second;
    auto funcIterator = std::next(_orderedFunctions.begin(), index);
    auto func = std::move(*funcIterator);
    _orderedFunctions.erase(funcIterator);
    _idToIndex.erase(it);
    for (auto &[first, second] : _idToIndex)
    {
        if (second > index)
        {
            second--;
        }
    }
    return func;
}
