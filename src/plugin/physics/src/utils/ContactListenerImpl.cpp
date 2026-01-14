#include "Physics.pch.hpp"

#include "utils/ContactListenerImpl.hpp"

#include "resource/EventManager.hpp"
#include "resource/PhysicsManager.hpp"

namespace Physics::Utils {

/**
 * EnTT divides the entity ID into two parts: the entity index and the version.
 * The entity index is the index of the entity in the registry, and the version part is a counter
 * that increments every time an entity with the same index is destroyed and recreated.
 * When recreating an entity from an integer, we need to make sure that both parts are preserved.
 *
 * With 32 bits the value of ENTITY_ID_MASK will be 0xFFFFFFFF.
 *
 * Note: we have to shift the entity mask to the left by the number of bits it has,
 * which means we have to compute the size of the type in bits (sizeof(type) * 8),
 * substract the number of bits used by the entity mask (we get it using popcount)
 * and then shift the entity mask to the left by that number of bits.
 */
static constexpr inline const uint32_t ENTITY_ID_MASK =
    entt::entt_traits<uint32_t>::entity_mask
        << (sizeof(uint32_t) * 8 -
            std::popcount(entt::entt_traits<uint32_t>::entity_mask)) |
    entt::entt_traits<uint32_t>::version_mask;

void ContactListenerImpl::OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2,
                                         const JPH::ContactManifold &, JPH::ContactSettings &)
{
    auto entity1 = static_cast<Engine::EntityId>(inBody1.GetUserData() & ENTITY_ID_MASK);
    auto entity2 = static_cast<Engine::EntityId>(inBody2.GetUserData() & ENTITY_ID_MASK);

    std::scoped_lock lock(_bufferMutex);
    _bufferedAdded.emplace_back(Physics::Event::CollisionAddedEvent{entity1, entity2});
}

void ContactListenerImpl::OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2,
                                             const JPH::ContactManifold &, JPH::ContactSettings &)
{
    auto entity1 = static_cast<Engine::EntityId>(inBody1.GetUserData() & ENTITY_ID_MASK);
    auto entity2 = static_cast<Engine::EntityId>(inBody2.GetUserData() & ENTITY_ID_MASK);

    std::scoped_lock lock(_bufferMutex);
    _bufferedPersisted.emplace_back(Physics::Event::CollisionPersistedEvent{entity1, entity2});
}

void ContactListenerImpl::OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair)
{
    auto &physicsManager = _core.GetResource<Physics::Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();

    JPH::Body *body1 = bodyInterface.TryGetBody(inSubShapePair.GetBody1ID());
    JPH::Body *body2 = bodyInterface.TryGetBody(inSubShapePair.GetBody2ID());
    if (body1 == nullptr || body2 == nullptr)
    {
        Log::Error("ContactListenerImpl: OnContactRemoved: body1 or body2 is nullptr, skipping callbacks.");
        return;
    }

    auto entity1 = static_cast<Engine::EntityId>(body1->GetUserData() & ENTITY_ID_MASK);
    auto entity2 = static_cast<Engine::EntityId>(body2->GetUserData() & ENTITY_ID_MASK);

    std::scoped_lock lock(_bufferMutex);
    _bufferedRemoved.emplace_back(Physics::Event::CollisionRemovedEvent{entity1, entity2});
}

void ContactListenerImpl::ProcessBufferedEvents(Engine::Core &core)
{
    std::vector<Physics::Event::CollisionAddedEvent> added;
    std::vector<Physics::Event::CollisionPersistedEvent> persisted;
    std::vector<Physics::Event::CollisionRemovedEvent> removed;
    {
        std::scoped_lock lock(_bufferMutex);
        added.swap(_bufferedAdded);
        persisted.swap(_bufferedPersisted);
        removed.swap(_bufferedRemoved);
    }

    if (added.empty() && persisted.empty() && removed.empty())
        return;

    auto &eventManager = core.GetResource<::Event::Resource::EventManager>();

    for (const auto &e : added)
    {
        if (core.IsEntityValid(e.entity1) && core.IsEntityValid(e.entity2))
            eventManager.PushEvent(e);
    }
    for (const auto &e : persisted)
    {
        if (core.IsEntityValid(e.entity1) && core.IsEntityValid(e.entity2))
            eventManager.PushEvent(e);
    }
    for (const auto &e : removed)
    {
        if (core.IsEntityValid(e.entity1) && core.IsEntityValid(e.entity2))
            eventManager.PushEvent(e);
    }
}

} // namespace Physics::Utils
