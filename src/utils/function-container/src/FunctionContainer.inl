#include "FunctionContainer.hpp"
#include "CallableFunction.hpp" // TODO: seems wrong to construct a callable fn?
#include "Logger.hpp"

template<typename TReturn, typename... TArgs>
template<typename TCallable>
void ES::Utils::FunctionContainer::FunctionContainer<TReturn, TArgs...>::AddFunction(TCallable callable)
{
    std::size_t id = 0;

    if constexpr (std::is_class_v<TCallable>)
    {
        id = typeid(callable).hash_code();
    }
    else
    {
        id = std::hash<TCallable>{}(callable);
    }

    if (_idToIndex.find(id) != _idToIndex.end())
    {
        ES::Utils::Log::Warn("Function already exists"); // TODO: be able to change container thing name
        return;
    }

    std::size_t index = _orderedFunctions.size();
    auto function = std::make_unique<CallableFunction<TCallable, TReturn, TArgs...>>(callable);
    _orderedFunctions.push_back(std::move(function));
    _idToIndex[id] = index;
}
