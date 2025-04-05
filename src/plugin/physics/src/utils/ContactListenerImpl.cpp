#include "ContactListenerImpl.hpp"

#include "PhysicsManager.hpp"

void ES::Plugin::Physics::Utils::ContactListenerImpl::OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2,
                                                                     const JPH::ContactManifold &inManifold,
                                                                     JPH::ContactSettings &ioSettings)
{
    auto entity1 = static_cast<ES::Engine::Entity>(inBody1.GetUserData());
    auto entity2 = static_cast<ES::Engine::Entity>(inBody2.GetUserData());

    for (auto &callback : _onContactAddedCallbacks)
    {
        callback(_core, entity1, entity2);
    }
}

void ES::Plugin::Physics::Utils::ContactListenerImpl::OnContactPersisted(const JPH::Body &inBody1,
                                                                         const JPH::Body &inBody2,
                                                                         const JPH::ContactManifold &inManifold,
                                                                         JPH::ContactSettings &ioSettings)
{
    auto entity1 = static_cast<ES::Engine::Entity>(inBody1.GetUserData());
    auto entity2 = static_cast<ES::Engine::Entity>(inBody2.GetUserData());

    for (auto &callback : _onContactPersistedCallbacks)
    {
        callback(_core, entity1, entity2);
    }
}

void ES::Plugin::Physics::Utils::ContactListenerImpl::OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair)
{
    auto &physicsManager = _core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyLockInterface();

    JPH::Body *body1 = bodyInterface.TryGetBody(inSubShapePair.GetBody1ID());
    JPH::Body *body2 = bodyInterface.TryGetBody(inSubShapePair.GetBody2ID());
    if (body1 == nullptr || body2 == nullptr)
    {
#ifdef ES_DEBUG
        ES::Utils::Log::Error("ContactListenerImpl: OnContactRemoved: body1 or body2 is nullptr, skipping callbacks.");
#endif
        return;
    }

    auto entity1 = static_cast<ES::Engine::Entity>(body1->GetUserData());
    auto entity2 = static_cast<ES::Engine::Entity>(body2->GetUserData());

    for (auto &callback : _onContactRemovedCallbacks)
    {
        callback(_core, entity1, entity2);
    }
}
