#pragma once

#include "Core.hpp"
#include "Entity.hpp"

namespace ES::Plugin::Physics::Utils {
/**
 * @brief Type erased contact callback.
 *
 * @note This should not be constructed directly, use the ContactCallback class instead.
 */
class IContactCallback {
  public:
    virtual ~IContactCallback() = default;

    /**
     * @brief Call the callback with the given core and entities.
     * @param core The core to use.
     * @param a The first entity.
     * @param b The second entity.
     */
    virtual void Call(ES::Engine::Core &core, const ES::Engine::Entity &a, const ES::Engine::Entity &b) const = 0;

};
} // namespace ES::Plugin::Physics::Utils