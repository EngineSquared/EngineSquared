#include "CallableFunction.hpp" // TODO: seems wrong to construct a callable fn?
#include "FunctionContainer.hpp"
#include "Logger.hpp"

template <typename TReturn, typename... TArgs>
template <typename TCallable>
void ES::Utils::FunctionContainer::FunctionContainer<TReturn, TArgs...>::AddFunction(TCallable callable)
{
    if (_idToIndex.find(CallableFunction<TCallable, TReturn, TArgs...>::GetCallableID(callable)) != _idToIndex.end())
    {
        ES::Utils::Log::Warn("Function already exists"); // TODO: be able to change container thing name
        return;
    }

    std::size_t index = _orderedFunctions.size();
    auto function = std::make_unique<CallableFunction<TCallable, TReturn, TArgs...>>(callable);
    _idToIndex[function->GetID()] = index;
    _orderedFunctions.push_back(std::move(function));
}

template <typename TReturn, typename... TArgs>
template <typename TCallable>
void ES::Utils::FunctionContainer::FunctionContainer<TReturn, TArgs...>::AddFunctionC(
    std::unique_ptr<BaseFunction<TCallable, TReturn, TArgs...>> &&function)
{
    unsigned int id = function->GetID();

    if (_idToIndex.find(id) != _idToIndex.end())
    {
        ES::Utils::Log::Warn("Function already exists"); // TODO: be able to change container thing name
        return;
    }

    std::size_t index = _orderedFunctions.size();
    _orderedFunctions.push_back(std::move(function));
    _idToIndex[id] = index;
}
