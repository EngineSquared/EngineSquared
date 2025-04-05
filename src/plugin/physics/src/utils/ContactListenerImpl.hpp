#pragma once

#include "Core.hpp"
#include "Entity.hpp"
#include "PhysicsManager.hpp"

#include <array>

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Collision/ContactListener.h>

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
                        JPH::ContactSettings &ioSettings) override
    {
        auto entity1 = static_cast<ES::Engine::Entity>(inBody1.GetUserData());
        auto entity2 = static_cast<ES::Engine::Entity>(inBody2.GetUserData());

        for (auto &callback : _onContactAddedCallbacks)
        {
            callback(_core, entity1, entity2);
        }
    }

    void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold,
                            JPH::ContactSettings &ioSettings) override
    {
        auto entity1 = static_cast<ES::Engine::Entity>(inBody1.GetUserData());
        auto entity2 = static_cast<ES::Engine::Entity>(inBody2.GetUserData());

        for (auto &callback : _onContactPersistedCallbacks)
        {
            callback(_core, entity1, entity2);
        }
    }

    void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override
    {
        // TODO: fix
        // auto &physicsManager = _core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
        // auto &bodyInterface = physicsManager.GetPhysicsSystem().GetBodyInterface();

        // auto entity1 = static_cast<ES::Engine::Entity>(bodyInterface.GetUserData(inSubShapePair.GetBody1ID()));
        // auto entity2 = static_cast<ES::Engine::Entity>(bodyInterface.GetUserData(inSubShapePair.GetBody2ID()));

        // for (auto &callback : _onContactRemovedCallbacks)
        // {
        //     callback(_core, entity1, entity2);
        // }
    }

    void AddOnContactAddedCallback(OnContactAddedCallback callback)
    {
        _onContactAddedCallbacks.push_back(std::move(callback));
    }

    void AddOnContactPersistedCallback(OnContactPersistedCallback callback)
    {
        _onContactPersistedCallbacks.push_back(std::move(callback));
    }

    void AddOnContactRemovedCallback(OnContactRemovedCallback callback)
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
