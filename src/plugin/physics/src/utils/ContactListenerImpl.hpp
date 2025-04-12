#pragma once

#include "Core.hpp"
#include "Entity.hpp"

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/ContactListener.h>

namespace ES::Plugin::Physics::Resource {
class PhysicsManager;
}

namespace ES::Plugin::Physics::Utils {
// ContactListener implementation
// This is used to define callbacks for contact events between bodies.
// Callbacks will be called with the Core, as well as the two entities that collided.
class ContactListenerImpl final : public JPH::ContactListener {
  public:
    using OnContactAddedCallback =
        std::function<void(ES::Engine::Core &, const ES::Engine::Entity &, const ES::Engine::Entity &)>;
    using OnContactPersistedCallback =
        std::function<void(ES::Engine::Core &, const ES::Engine::Entity &, const ES::Engine::Entity &)>;
    using OnContactRemovedCallback =
        std::function<void(ES::Engine::Core &, const ES::Engine::Entity &, const ES::Engine::Entity &)>;

    ContactListenerImpl() = delete;

    explicit ContactListenerImpl(ES::Engine::Core &core) : _core(core) {}
    ~ContactListenerImpl() override = default;

    JPH::ValidateResult OnContactValidate([[maybe_unused]] const JPH::Body &inBody1,
                                          [[maybe_unused]] const JPH::Body &inBody2,
                                          [[maybe_unused]] JPH::RVec3Arg inBaseOffset,
                                          [[maybe_unused]] const JPH::CollideShapeResult &inCollisionResult)
    {
        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold,
                        JPH::ContactSettings &ioSettings) override;

    void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold,
                            JPH::ContactSettings &ioSettings) override;

    void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override;

    /**
     * @brief Add a callback for when a contact between two entities is added.
     * @param callback The callback to add.
     * @note The callback will be called with the Core, as well as the two entities that collided.
     * @note The callback will be called once for each contact added.
     */
    inline void AddOnContactAddedCallback(OnContactAddedCallback callback)
    {
        _onContactAddedCallbacks.push_back(std::move(callback));
    }

    /**
     * @brief Add a callback for when a contact between two entities persist.
     * @param callback The callback to add.
     * @note The callback will be called with the Core, as well as the two entities that collided.
     * @note The callback won't be called for the first collision.
     * @note The callback will be called every frame until the contact is removed.
     */
    inline void AddOnContactPersistedCallback(OnContactPersistedCallback callback)
    {
        _onContactPersistedCallbacks.push_back(std::move(callback));
    }

    /**
     * @brief Add a callback for when a contact between two entities is removed.
     * @param callback The callback to add.
     * @note The callback will be called with the Core, as well as the two entities that collided.
     * @note The callback will be called once for each contact removed.
     */
    inline void AddOnContactRemovedCallback(OnContactRemovedCallback callback)
    {
        _onContactRemovedCallbacks.push_back(std::move(callback));
    }

  private:
    ES::Engine::Core &_core;

    std::vector<OnContactAddedCallback> _onContactAddedCallbacks;
    std::vector<OnContactPersistedCallback> _onContactPersistedCallbacks;
    std::vector<OnContactRemovedCallback> _onContactRemovedCallbacks;
};
} // namespace ES::Plugin::Physics::Utils
