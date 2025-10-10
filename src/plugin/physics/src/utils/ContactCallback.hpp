#pragma once

#include "FunctionContainer.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"

#include <functional>

namespace Plugin::Physics::Utils {
/**
 * @brief A utility class for handling contact callbacks in a physics engine.
 *
 * @tparam Components The components to check for in the entities involved in the contact.
 *
 * @note Callbacks will be called with the Core, as well as the two entities that collided.
 * @note The callback will be called once for each contact added.
 * @note The callback will be called only if the entities have the specified components.
 * @note If no components are specified, the callback will be called for all contacts.
 * @note If one component is specified, the callback will be called only if both entities have that component.
 * @note If two components are specified, the callback will be called only if one entity has the first component
 * and the other entity has the second component.
 */
using BaseCallback = Utils::FunctionContainer::BaseFunction<void, Engine::Core &, Engine::Entity &, Engine::Entity &>;
template <typename... Components> class ContactCallback : public BaseCallback {
  public:
    using CallbackFunc = std::function<void(Engine::Core &, Engine::Entity &, Engine::Entity &)>;

    explicit ContactCallback(CallbackFunc cb) : _callback(std::move(cb)) {}

    void operator()(Engine::Core &core, Engine::Entity &a, Engine::Entity &b) const final
    {
        static_assert(sizeof...(Components) <= 2, "ContactCallback can only have up to 2 components.");

        if constexpr (sizeof...(Components) == 0)
        {
            _callback(core, a, b);
        }
        else if constexpr (sizeof...(Components) == 1)
        {
            if (hasAllComponents<Components...>(core, a) && hasAllComponents<Components...>(core, b))
            {
                _callback(core, a, b);
            }
        }
        else if constexpr (sizeof...(Components) == 2)
        {
            callIfComponentMatch<Components...>(core, a, b);
        }
    }

    Utils::FunctionContainer::FunctionID GetID() const final { return _callback.target_type().hash_code(); }

  private:
    template <typename... Cs> inline bool hasAllComponents(Engine::Core &core, const Engine::Entity &entity) const
    {
        return ((entity.HasComponents<Cs>(core) && ...));
    }

    template <typename C1, typename C2>
    void callIfComponentMatch(Engine::Core &core, Engine::Entity &a, Engine::Entity &b) const
    {
        if ((a.HasComponents<C1>(core) && b.HasComponents<C2>(core)))
        {
            _callback(core, a, b);
        }
        else if ((a.HasComponents<C2>(core) && b.HasComponents<C1>(core)))
        {
            _callback(core, b, a);
        }
    }

    CallbackFunc _callback; ///< The callback function to call.
};
} // namespace Plugin::Physics::Utils
