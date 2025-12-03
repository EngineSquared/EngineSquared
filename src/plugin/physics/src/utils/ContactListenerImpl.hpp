#pragma once

#include "core/Core.hpp"
#include "entity/Entity.hpp"

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <mutex>
#include <vector>

#include "event/CollisionEvent.hpp"

namespace Physics::Resource {
class PhysicsManager;
}

namespace Physics::Utils {

/**
 * @brief ContactListener implementation
 *
 * This is used to define callbacks for contact events between bodies.
 * Callbacks will be called with the Core, as well as the two entities that collided.
 *
 * @note Jolt will call these callbacks from physics worker threads. To avoid
 * concurrent access to engine resources (like the entt registry) and the
 * EventManager, events are buffered in worker threads and flushed to the
 * main thread by calling `ProcessBufferedEvents(core)` from the main
 * physics update system. Do NOT call EventManager::PushEvent() directly in
 * these callbacks.
 */
class ContactListenerImpl final : public JPH::ContactListener {
  public:
    ContactListenerImpl() = delete;

    explicit ContactListenerImpl(Engine::Core &core) : _core(core) {}
    ~ContactListenerImpl() override = default;

    JPH::ValidateResult OnContactValidate([[maybe_unused]] const JPH::Body &inBody1,
                                          [[maybe_unused]] const JPH::Body &inBody2,
                                          [[maybe_unused]] JPH::RVec3Arg inBaseOffset,
                                          [[maybe_unused]] const JPH::CollideShapeResult &inCollisionResult) override
    {
        return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
    }

    /**
     * @brief Called when two bodies start to make contact.
     *
     * Right now we use 32 bits for entities IDs with EnTT but Jolt stores user data as 64 bits
     * so we have to mask the upper 32 bits
     * auto entity1 = static_cast<Engine::Entity>(inBody1.GetUserData() & ENTITY_ID_MASK);
     * auto entity2 = static_cast<Engine::Entity>(inBody2.GetUserData() & ENTITY_ID_MASK);
     *
     * Validate entity ids and buffer the event on a worker thread; it will be flushed on
     * the main thread by the PhysicsUpdate system.
     *
     * Buffer the event. Can't safely access the registry from a physics worker thread,
     * so defer any entity validity checks until ProcessBufferedEvents (main thread).
     *
     * @param inBody1 The first body involved in the contact.
     * @param inBody2 The second body involved in the contact.
     * @param inManifold The contact manifold containing contact points.
     * @param ioSettings Contact settings that can be modified.
     */
    void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold,
                        JPH::ContactSettings &ioSettings) override;

    /**
     * @brief Called when two bodies continue to make contact.
     *
     * Same as OnContactAdded: delay entity validation until ProcessBufferedEvents (main thread)
     *
     * @param inBody1 The first body involved in the contact.
     * @param inBody2 The second body involved in the contact.
     * @param inManifold The contact manifold containing contact points.
     * @param ioSettings Contact settings that can be modified.
     */
    void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold,
                            JPH::ContactSettings &ioSettings) override;

    /**
     * @brief Called when two bodies cease to make contact.
     *
     * Buffer the event on a worker thread; it will be flushed on the main thread by the PhysicsUpdate system.
     *
     * @param inSubShapePair The pair of sub-shapes that were in contact.
     */
    void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override;

    /**
     * @brief Flush buffered events to the main thread EventManager (should be called from main thread)
     *
     * Move buffers into local copies under lock then push events to the EventManager from main thread.
     */
    void ProcessBufferedEvents(Engine::Core &core);

  private:
    Engine::Core &_core;
    std::mutex _bufferMutex;
    std::vector<Physics::Event::CollisionAddedEvent> _bufferedAdded;
    std::vector<Physics::Event::CollisionPersistedEvent> _bufferedPersisted;
    std::vector<Physics::Event::CollisionRemovedEvent> _bufferedRemoved;
};

} // namespace Physics::Utils
