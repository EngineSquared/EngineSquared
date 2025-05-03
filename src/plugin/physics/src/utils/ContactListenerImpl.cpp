#include "ContactListenerImpl.hpp"

#include "PhysicsManager.hpp"

#include <bit>
#include <entt/entity/entity.hpp>

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
    entt::entt_traits<ES::Engine::Entity::entity_id_type>::entity_mask
        << (sizeof(ES::Engine::Entity::entity_id_type) * 8 -
            std::popcount(entt::entt_traits<ES::Engine::Entity::entity_id_type>::entity_mask)) |
    entt::entt_traits<ES::Engine::Entity::entity_id_type>::version_mask;

void ES::Plugin::Physics::Utils::ContactListenerImpl::OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2,
                                                                     const JPH::ContactManifold &,
                                                                     JPH::ContactSettings &)
{
    if (_onContactAddedCallbacks.IsEmpty())
    {
        return;
    }

    // Right now we use 32 bits for entities IDs with EnTT but Jolt stores user data as 64 bits
    // so we have to mask the upper 32 bits
    auto entity1 = static_cast<ES::Engine::Entity>(inBody1.GetUserData() & ENTITY_ID_MASK);
    auto entity2 = static_cast<ES::Engine::Entity>(inBody2.GetUserData() & ENTITY_ID_MASK);

    for (auto &callback : _onContactAddedCallbacks.GetFunctions())
    {
        callback->Call(_core, entity1, entity2);
    }
}

void ES::Plugin::Physics::Utils::ContactListenerImpl::OnContactPersisted(const JPH::Body &inBody1,
                                                                         const JPH::Body &inBody2,
                                                                         const JPH::ContactManifold &,
                                                                         JPH::ContactSettings &)
{
    if (_onContactPersistedCallbacks.IsEmpty())
    {
        return;
    }

    auto entity1 = static_cast<ES::Engine::Entity>(inBody1.GetUserData() & ENTITY_ID_MASK);
    auto entity2 = static_cast<ES::Engine::Entity>(inBody2.GetUserData() & ENTITY_ID_MASK);

    for (auto &callback : _onContactPersistedCallbacks.GetFunctions())
    {
        callback->Call(_core, entity1, entity2);
    }
}

void ES::Plugin::Physics::Utils::ContactListenerImpl::OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair)
{
    if (_onContactRemovedCallbacks.IsEmpty())
    {
        return;
    }

    auto &physicsManager = _core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();

    JPH::Body *body1 = bodyInterface.TryGetBody(inSubShapePair.GetBody1ID());
    JPH::Body *body2 = bodyInterface.TryGetBody(inSubShapePair.GetBody2ID());
    if (body1 == nullptr || body2 == nullptr)
    {
        ES::Utils::Log::Error("ContactListenerImpl: OnContactRemoved: body1 or body2 is nullptr, skipping callbacks.");
        return;
    }

    auto entity1 = static_cast<ES::Engine::Entity>(body1->GetUserData() & ENTITY_ID_MASK);
    auto entity2 = static_cast<ES::Engine::Entity>(body2->GetUserData() & ENTITY_ID_MASK);

    for (auto &callback : _onContactRemovedCallbacks.GetFunctions())
    {
        callback->Call(_core, entity1, entity2);
    }
}
