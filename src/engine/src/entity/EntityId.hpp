#pragma once

#include "Id.hpp"
#include "core/Core.hpp"
#include <fmt/format.h>

namespace Engine {
/// @struct EntityId
/// @brief Represents a unique identifier for an entity in the Engine's entity-component system.
///     An EntityId is just Id with utility methods to manage components associated with the entity but not the core.
/// @note EntityId maintains the same memory footprint as its underlying ValueType (Engine::Id).
/// @see Engine::Id
/// @see Engine::Core
struct EntityId : Id {
    /// @brief The underlying value type of the EntityId, which is the same as Engine::Id::ValueType.
    using ValueType = Id::ValueType;

    /// @brief Constructs an EntityId with the given value. By default, it constructs a null EntityId.
    /// @param v The value to initialize the EntityId with. If not provided, it defaults to the null value defined by
    ///     Engine::Id.
    /// @todo rename v to value
    /// @todo put the implementation in the cpp file
    constexpr EntityId(ValueType v = NullValue()) : Id{v} {}

    /// @brief Constructs an EntityId from an Engine::Id. This allows for implicit conversion from Engine::Id to
    ///     EntityId.
    /// @param id The Engine::Id to convert.
    /// @todo put the implementation in the cpp file
    constexpr EntityId(Id id) : Id{id} {}

    /// @brief Implicit conversion operator to the underlying ValueType. This allows for seamless use of EntityId where
    ///     a ValueType is expected.
    /// @todo put the implementation in the cpp file
    constexpr operator ValueType() const { return value; }

    /// @brief Returns a null EntityId. A null EntityId is an EntityId that does not correspond to any valid entity in
    ///     the registry.
    /// @return A null EntityId.
    /// @todo put the implementation in the cpp file
    static constexpr EntityId Null() { return EntityId{NullValue()}; }

    /// @brief Checks if the EntityId is valid. An EntityId is considered valid if it corresponds to an existing entity
    ///     in the Core's registry.
    /// @param core The Core instance to check the validity against.
    /// @return true if the EntityId is valid, false otherwise.
    /// @todo put the implementation in the cpp file, (remove inline)
    inline bool IsValid(const Engine::Core &core) const { return core.IsEntityValid(value); }

    /// @brief Add a component to an entity.
    /// @tparam TComponent The type of the component to add to the registry.
    /// @param core The Core instance whose registry is used to store the component.
    /// @param component The rvalue of the component to add to the registry.
    /// @return A reference to the added component.
    /// @todo put the implementation in the inl file
    template <typename TComponent> inline decltype(auto) AddComponent(Engine::Core &core, TComponent &&component)
    {
        /// @todo put the log in the core emplace
        Log::Debug(fmt::format("[EntityID:{}] AddComponent: {}", value, typeid(TComponent).name()));
        return core.GetRegistry().emplace<TComponent>(value, std::forward<TComponent>(component));
    }

    /// @brief Add a component to an entity.
    /// @tparam TComponent The type of the component to add to the registry.
    /// @tparam TArgs The types of the arguments to construct the component in-place in the registry.
    /// @param core The Core instance whose registry is used to store the component.
    /// @param args The arguments to construct the component in-place in the registry.
    /// @return A reference to the added component.
    /// @todo put the implementation in the inl file
    template <typename TComponent, typename... TArgs>
    inline decltype(auto) AddComponent(Engine::Core &core, TArgs &&...args)
    {
        /// @todo put the log in the core emplace
        Log::Debug(fmt::format("[EntityID:{}] AddComponent: {}", value, typeid(TComponent).name()));
        return core.GetRegistry().emplace<TComponent>(value, std::forward<TArgs>(args)...);
    }

    /// @brief Add a component to an entity if it does not already exist.
    /// @tparam TComponent type to add to registry.
    /// @tparam TArgs type used to create the component.
    /// @param core registry used to store the component.
    /// @param args parameters used to instanciate component directly in registry memory.
    /// @return reference of the added component.
    /// @todo put the implementation in the inl file.
    template <typename TComponent, typename... TArgs>
    inline decltype(auto) AddComponentIfNotExists(Engine::Core &core, TArgs &&...args)
    {
        if (this->HasComponents<TComponent>(core))
        {
            return this->GetComponents<TComponent>(core);
        }
        return this->AddComponent<TComponent>(core, std::forward<TArgs>(args)...);
    }

    /// @brief Check if entity have one or multiple component's type.
    /// @tparam TComponent components to check.
    /// @param core context used to check if the entity have the component in the core's registry.
    /// @return true if entity have all requested component, false otherwise.
    template <typename... TComponent> inline bool HasComponents(const Engine::Core &core) const
    {
        return core.GetRegistry().all_of<TComponent...>(value);
    }

    /// @brief Add a temporary component to an entity. Temporary component are removed when calling
    ///     RemoveTemporaryComponents system.
    /// @tparam TTempComponent type to add to registry
    /// @tparam TArgs type used to create the component
    /// @param core registry used to store the component
    /// @param args parameters used to instanciate component directly in registry memory
    /// @return reference of the added component
    /// @see Engine::EntityId::RemoveTemporaryComponents
    /// @todo put the implementation in the inl file
    template <typename TTempComponent, typename... TArgs>
    inline decltype(auto) AddTemporaryComponent(Engine::Core &core, TArgs &&...args)
    {
        if (!temporaryComponent.contains(std::type_index(typeid(TTempComponent))))
        {
            temporaryComponent[std::type_index(typeid(TTempComponent))] = [](Core &c) {
                Log::Debug(fmt::format("RemoveTemporaryComponent: {}", typeid(TTempComponent).name()));
                c.GetRegistry().clear<TTempComponent>();
            };
        }

        return this->AddComponent<TTempComponent>(core, std::forward<TArgs>(args)...);
    }

    /// @brief Get components of type TComponent from an entity.
    /// @tparam TComponent The type of the component to get from the registry.
    /// @param core The Core instance whose registry is used to store the component.
    /// @return The components of type TComponent from the entity.
    /// @todo put the implementation in the inl file
    template <typename... TComponent> inline decltype(auto) GetComponents(Engine::Core &core)
    {
        return core.GetRegistry().get<TComponent...>(this->value);
    }

    /// @brief Get components of type TComponent from the entity.
    /// @tparam TComponent The type of the component to get from the registry.
    /// @param core The Core instance whose registry is used to store the component.
    /// @return The components of type TComponent from the entity.
    /// @todo put the implementation in the inl file
    template <typename... TComponent> inline decltype(auto) GetComponents(Engine::Core &core) const
    {
        return core.GetRegistry().get<TComponent...>(this->value);
    }

    /// @brief Try to get a component of type TComponent from the entity. It returns a pointer to the component if it
    ///     exists, or nullptr if it does not exist.
    /// @tparam TComponent The type of the component to get from the registry.
    /// @param core The Core instance whose registry is used to store the component.
    /// @return A pointer to the component of type TComponent from the entity if it exists, or nullptr if it does not
    /// exist.
    /// @todo put the implementation in the inl file
    template <typename TComponent> inline decltype(auto) TryGetComponent(Engine::Core &core)
    {
        return core.GetRegistry().try_get<TComponent>(this->value);
    }

    /// @brief Remove a component from an entity.
    /// @tparam TComponent The type of the component to remove from the registry.
    /// @param core The Core instance whose registry is used to store the component.
    /// @todo put the implementation in the inl file
    template <typename TComponent> inline void RemoveComponent(Engine::Core &core)
    {
        Log::Debug(fmt::format("[EntityID:{}] RemoveComponent: {}", value, typeid(TComponent).name()));
        core.GetRegistry().remove<TComponent>(value);
    }

    /// @brief Remove all temporary components from the registry. This system should be called at the end of each loop
    ///     to ensure that temporary components are removed after they have been used.
    /// @param core The Core instance whose registry is used to store the components.
    /// @todo put the implementation in the inl file
    /// @see Engine::EntityId::AddTemporaryComponent
    static void RemoveTemporaryComponents(Core &core)
    {
        if (temporaryComponent.empty())
        {
            return;
        }
        for (const auto &[typeIndex, func] : temporaryComponent)
        {
            func(core);
        }
        temporaryComponent.clear();
    }

  private:
    inline static std::unordered_map<std::type_index, std::function<void(Core &)>> temporaryComponent = {};
};

/// @brief Static assertion to ensure that the size of EntityId is equal to the size of its underlying ValueType. This
///     is important to ensure that EntityId does not have any additional memory overhead compared to its underlying
///     ValueType, which is crucial for performance in an entity-component-system where many EntityId instances may be
///     created and stored
static_assert(sizeof(EntityId) == sizeof(EntityId::ValueType), "EntityId size must be equal to Id size");
} // namespace Engine

/// @struct std::hash<Engine::EntityId>
/// @brief Specialization of std::hash for Engine::EntityId to allow it to be used as a key in data containers like
///     std::unordered_map.
template <> struct std::hash<Engine::EntityId> {
    /// @brief Hash function for EntityId. It uses the hash of the underlying ValueType to compute the hash of the
    ///     EntityId.
    /// @param entityId The EntityId to hash.
    /// @return The hash of the EntityId.
    /// @todo put the implementation in the cpp file
    std::size_t operator()(const Engine::EntityId &entityId) const noexcept
    {
        return std::hash<entt::id_type>{}(entityId.value);
    }
};

/// @struct fmt::formatter<Engine::EntityId>
/// @brief Specialization of fmt::formatter for Engine::EntityId to allow it to be formatted using the {fmt} library.
template <> struct fmt::formatter<Engine::EntityId> : fmt::formatter<std::string> {
    /// @brief Formats an EntityId for output. If the EntityId is null, it will format as "null_entity". Otherwise, it
    ///     will format using the Log::EntityToDebugString function to provide a human-readable representation of the
    ///     EntityId.
    /// @tparam FormatContext The type of the format context used by the {fmt} library.
    /// @param entityId The EntityId to format.
    /// @param ctx The format context used by the {fmt} library.
    /// @return The formatted string representation of the EntityId.
    /// @todo put the implementation in the inl file
    template <typename FormatContext> auto format(const Engine::EntityId &entityId, FormatContext &ctx) const
    {
        if (entityId.IsNull())
        {
            return fmt::formatter<std::string>::format("null_entity", ctx);
        }
        return fmt::formatter<std::string>::format(Log::EntityToDebugString(entityId.value), ctx);
    }
};
