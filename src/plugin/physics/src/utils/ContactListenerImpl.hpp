#pragma once

#include "ContactCallback.hpp"
#include "Core.hpp"
#include "Entity.hpp"
#include "FunctionContainer.hpp"

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
    template <typename... Components>
    inline ES::Utils::FunctionContainer::FunctionID AddOnContactAddedCallback(std::unique_ptr<BaseCallback> &&callback)
    {
        return _onContactAddedCallbacks.AddFunction(std::move(callback));
    }

    /**
     * @brief Add a callback for when a contact between two entities persist.
     * @param callback The callback to add.
     * @note The callback will be called with the Core, as well as the two entities that collided.
     * @note The callback won't be called for the first collision.
     * @note The callback will be called every frame until the contact is removed.
     */
    template <typename... Components>
    inline ES::Utils::FunctionContainer::FunctionID
    AddOnContactPersistedCallback(std::unique_ptr<BaseCallback> &&callback)
    {
        return _onContactPersistedCallbacks.AddFunction(std::move(callback));
    }

    /**
     * @brief Add a callback for when a contact between two entities is removed.
     * @param callback The callback to add.
     * @note The callback will be called with the Core, as well as the two entities that collided.
     * @note The callback will be called once for each contact removed.
     */
    template <typename... Components>
    inline ES::Utils::FunctionContainer::FunctionID
    AddOnContactRemovedCallback(std::unique_ptr<BaseCallback> &&callback)
    {
        return _onContactRemovedCallbacks.AddFunction(std::move(callback));
    }

    /**
     * @brief Remove a callback for when a contact between two entities is added.
     * @param id The ID of the callback to remove.
     * @return True if the callback was removed, false otherwise.
     */
    inline bool RemoveOnContactAddedCallback(ES::Utils::FunctionContainer::FunctionID id)
    {
        return _onContactAddedCallbacks.DeleteFunction(id);
    }

    /**
     * @brief Remove a callback for when a contact between two entities persist.
     * @param id The ID of the callback to remove.
     * @return True if the callback was removed, false otherwise.
     */
    inline bool RemoveOnContactPersistedCallback(ES::Utils::FunctionContainer::FunctionID id)
    {
        return _onContactPersistedCallbacks.DeleteFunction(id);
    }

    /**
     * @brief Remove a callback for when a contact between two entities is removed.
     * @param id The ID of the callback to remove.
     * @return True if the callback was removed, false otherwise.
     */
    inline bool RemoveOnContactRemovedCallback(ES::Utils::FunctionContainer::FunctionID id)
    {
        return _onContactRemovedCallbacks.DeleteFunction(id);
    }

  private:
    using CallbackContainer =
        ES::Utils::FunctionContainer::FunctionContainer<void, ES::Engine::Core &, ES::Engine::Entity &,
                                                        ES::Engine::Entity &>;
    ES::Engine::Core &_core;

    CallbackContainer _onContactAddedCallbacks;     ///< Callbacks for when a contact is added.
    CallbackContainer _onContactPersistedCallbacks; ///< Callbacks for when a contact is persisted.
    CallbackContainer _onContactRemovedCallbacks;   ///< Callbacks for when a contact is removed.
};
} // namespace ES::Plugin::Physics::Utils
