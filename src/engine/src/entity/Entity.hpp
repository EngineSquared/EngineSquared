#pragma once

#include "EntityToIDString.hpp"
#include "Id.hpp"
#include "Logger.hpp"
#include "core/Core.hpp"
#include "entity/EntityId.hpp"
#include <entt/entt.hpp>
#include <typeindex>

namespace Engine {

/// @class Entity
/// @brief Wrapper class providing a convenient interface for entity manipulation with the Core.
///     Entity acts as a handle to an entity in the registry, combining a Core reference with an EntityId.
///     It provides utility methods to add, remove, and query components, and maintains context relative
///     to the Core it belongs to.
/// @see Engine::Core
/// @see Engine::EntityId
class Entity {
  public:
    /// @brief Create an Entity from EntityId and the Core it belongs to.
    /// @param core Reference to the Core instance that the Entity belongs to.
    /// @param entityId The EntityId that identifies the entity in the Core's registry.
    /// @todo put the implementation in the cpp file
    Entity(Core &core, EntityId entityId) : _core(core), _entityId(entityId) {}

    /// @brief Default destructor for Entity. It does not perform any special cleanup, as the Core is responsible for
    ///     managing the entity's lifecycle.
    ~Entity() = default;

    /// @brief Check if the entity is alive.
    /// @return true if the entity is alive, false otherwise.
    bool IsAlive() const;

    /// @brief Get the EntityId associated with this Entity.
    /// @return The EntityId of this Entity.
    /// @todo put the implementation in the cpp file, (remove inline)
    inline EntityId Id() const { return _entityId; }

    /// @brief Implicit conversion operator to EntityId.
    /// @return The EntityId of this Entity.
    /// @todo put the implementation in the cpp file, (remove inline)
    explicit(false) inline operator EntityId() const { return _entityId; }

    /// @brief Kill the entity, removing all components and making it invalid. After calling this method, the EntityId
    ///     will no longer correspond to a valid entity in the Core's registry, and the Entity will be considered dead.
    void Kill();

    /// @brief Add a component to an entity.
    /// @tparam TComponent The type of the component to add to the registry.
    /// @param component The rvalue of the component to add to the registry.
    /// @return A reference to the added component.
    /// @todo put the implementation in the inl file
    template <typename TComponent> inline decltype(auto) AddComponent(TComponent &&component)
    {
        return _entityId.AddComponent(GetCore(), std::forward<TComponent>(component));
    }

    /// @brief Add a component to an entity.
    /// @tparam TComponent The type of the component to add to the registry.
    /// @tparam TArgs The types of the arguments to construct the component in-place in the registry.
    /// @param args The arguments to construct the component in-place in the registry.
    /// @return A reference to the added component.
    /// @todo put the implementation in the inl file
    template <typename TComponent, typename... TArgs> inline decltype(auto) AddComponent(TArgs &&...args)
    {
        return _entityId.AddComponent<TComponent>(GetCore(), std::forward<TArgs>(args)...);
    }

    /// @brief Add a component to an entity if it does not already exist.
    /// @tparam TComponent type to add to registry.
    /// @tparam TArgs type used to create the component.
    /// @param args parameters used to instanciate component directly in registry memory.
    /// @return reference of the added component.
    /// @todo put the implementation in the inl file.
    template <typename TComponent, typename... TArgs> inline decltype(auto) AddComponentIfNotExists(TArgs &&...args)
    {
        return _entityId.AddComponentIfNotExists<TComponent>(GetCore(), std::forward<TArgs>(args)...);
    }

    /// @brief Add a temporary component to an entity. Temporary component are removed when calling
    ///     RemoveTemporaryComponents system.
    /// @tparam TTempComponent type to add to registry
    /// @tparam TArgs type used to create the component
    /// @param args parameters used to instanciate component directly in registry memory
    /// @return reference of the added component
    /// @see Engine::EntityId::RemoveTemporaryComponents
    template <typename TTempComponent, typename... TArgs> inline decltype(auto) AddTemporaryComponent(TArgs &&...args)
    {
        return _entityId.AddTemporaryComponent<TTempComponent>(GetCore(), std::forward<TArgs>(args)...);
    }

    /// @brief Remove all temporary components from the registry.
    /// @param core The Core instance whose registry is used to store the component.
    /// @see Engine::EntityId::AddTemporaryComponent
    /// @todo put the implementation in the cpp file, (remove inline)
    static inline void RemoveTemporaryComponents(Core &core) { EntityId::RemoveTemporaryComponents(core); }

    /// @brief Remove a component from an entity.
    /// @tparam TComponent The type of the component to remove from the registry.
    /// @todo put the implementation in the inl file
    template <typename TComponent> inline void RemoveComponent() { _entityId.RemoveComponent<TComponent>(GetCore()); }

    /// @brief Check if entity have one or multiple component's type.
    /// @tparam TComponent components to check.
    /// @return true if entity have all requested component, false otherwise.
    /// @todo put the implementation in the inl file
    template <typename... TComponent> inline bool HasComponents() const
    {
        return _entityId.HasComponents<TComponent...>(GetCore());
    }

    /// @brief Get components of type TComponent from the entity.
    /// @tparam TComponent components to get.
    /// @return components of type TComponent from the entity.
    /// @todo put the implementation in the inl file
    template <typename... TComponent> inline decltype(auto) GetComponents()
    {
        return _entityId.GetComponents<TComponent...>(GetCore());
    }

    /// @brief Get components of type TComponent from the entity.
    /// @tparam TComponent components to get.
    /// @return components of type TComponent from the entity.
    /// @todo put the implementation in the inl file
    template <typename... TComponent> inline decltype(auto) GetComponents() const
    {
        return _entityId.GetComponents<TComponent...>(GetCore());
    }

    /// @brief Try to get a component of type TComponent from the entity. It returns a pointer to the component if it
    ///     exists, or nullptr if it does not exist.
    /// @tparam TComponent The type of the component to get from the entity.
    /// @return The component of type TComponent from the entity if it exists, or nullptr if it does not exist.
    /// @todo put the implementation in the inl file
    template <typename TComponent> inline decltype(auto) TryGetComponent()
    {
        return _entityId.TryGetComponent<TComponent>(GetCore());
    }

    /// @brief Equality operator for Entity. It compares the underlying EntityId values to determine if two Entity
    ///     instances refer to the same entity in the registry.
    /// @param rhs The other Entity to compare with.
    /// @return true if the entities are equal, false otherwise.
    /// @todo put the implementation in the cpp file
    bool operator==(const Entity &rhs) const { return _entityId.value == rhs._entityId.value; }

    /// @brief Equality operator for Entity and EntityId. It compares the underlying EntityId value of the Entity with
    ///     the EntityId to determine if they refer to the same entity in the registry.
    /// @param rhs The EntityId to compare with.
    /// @return true if the Entity and EntityId are equal, false otherwise.
    /// @todo put the implementation in the cpp file
    bool operator==(const EntityId &rhs) const { return _entityId.value == rhs.value; }

  private:
    /// @brief Get a reference to the Core instance that this Entity belongs to. This is used internally to access the
    ///     registry and other resources when manipulating components.
    /// @return A reference to the Core instance.
    /// @todo put the implementation in the cpp file
    constexpr Core &GetCore() const { return _core.get(); }

    /// @brief A reference to the Core instance that this Entity belongs to.
    std::reference_wrapper<Core> _core;
    /// @brief The EntityId that identifies the entity in the Core's registry. This is the underlying identifier for
    ///     the entity that this Entity class wraps around, and is used for all operations that manipulate the entity in
    ///     the registry.
    EntityId _entityId;
};

} // namespace Engine

/// @brief Specialization of fmt::formatter for Engine::Entity to allow it to be formatted as a string using the fmt
///     library. The format will be the same as the underlying EntityId, which will result in a human-readable
///     representation of the entity's identifier.
template <> struct fmt::formatter<Engine::Entity> : fmt::formatter<Engine::EntityId> {
    /// @brief Format an Entity for output using the fmt library. It formats the Entity by formatting its underlying
    ///     EntityId, which will result in a human-readable representation of the entity's identifier.
    /// @tparam FormatContext The type of the format context used by the fmt library.
    /// @param entity The Entity to format.
    /// @param ctx The format context used by the fmt library.
    /// @return The formatted string representation of the Entity.
    /// @todo put the implementation in the inl file
    template <typename FormatContext> auto format(const Engine::Entity &entity, FormatContext &ctx) const
    {
        return fmt::formatter<Engine::EntityId>::format(entity.Id(), ctx);
    }
};
