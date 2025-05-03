#include "System.hpp"

template <typename TCallable>
std::shared_ptr<ES::Engine::SystemBase> ES::Engine::SystemContainer::AddSystem(TCallable callable)
{
    std::size_t id = 0;

    if constexpr (std::is_class_v<TCallable>)
    {
        id = typeid(callable).hash_code();
        ES::Utils::Log::Info(fmt::format("Adding system with name: {}", typeid(callable).name()));
    }
    else
    {
        id = std::hash<TCallable>{}(callable);
        ES::Utils::Log::Info(fmt::format("Adding system with hash: {}", id));
    }

    if (_idToIndex.contains(id))
    {
        if constexpr (std::is_class_v<TCallable>)
        {
            ES::Utils::Log::Warn(fmt::format("System already exists: {}", typeid(callable).name()));
        }
        else
        {
            ES::Utils::Log::Warn(fmt::format("System already exists: {}", id));
        }
        return nullptr;
    }
    std::size_t index = _orderedSystems.size();
    auto system = std::make_shared<System<TCallable>>(callable);
    _orderedSystems.push_back(system);
    _idToIndex[id] = index;
    return _orderedSystems.back();
}
