#include "System.hpp"

template <typename TCallable> void ES::Engine::SystemContainer::AddSystem(TCallable callable)
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

    if (_idToIndex.find(static_cast<entt::id_type>(id)) != _idToIndex.end())
    {
        ES::Utils::Log::Warn("System already exists");
        return;
    }

    auto index = static_cast<unsigned int>(_orderedSystems.size());
    auto system = std::make_unique<System<TCallable>>(callable);
    _orderedSystems.push_back(std::move(system));
    _idToIndex[id] = index;
}
