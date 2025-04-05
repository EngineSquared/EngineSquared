#pragma once

#include <memory>

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

#include "ContactListenerImpl.hpp"

#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

namespace ES::Plugin::Physics::Resource {
/**
 * PhysicsManager is a resource that wraps necessary JoltPhysics components.
 */
class PhysicsManager {
  public:
    /**
     * Constructor.
     */
    PhysicsManager();

    /**
     * Destructor.
     */
    ~PhysicsManager() = default;

    /**
     * @brief Initialize the physics system.
     *
     * @param core A reference to the core engine, used for the contact listener.
     *
     * @return void
     */
    void Init(ES::Engine::Core &core);

    /**
     * @brief Get a reference to the physics system.
     *
     * @return JPH::PhysicsSystem&
     */
    inline JPH::PhysicsSystem &GetPhysicsSystem() { return *_physicsSystem; }

    /**
     * @brief Get a pointer to the temp allocator.
     *
     * @return JPH::TempAllocator*
     * @note A raw pointer is returned for ease of use with JoltPhysics APIs.
     * Memory ownership is managed by the PhysicsManager.
     */
    inline JPH::TempAllocator *GetTempAllocator() { return _tempAllocator.get(); }

    /**
     * @brief Get a pointer to the job system.
     *
     * @return JPH::JobSystem*
     * @note A raw pointer is returned for ease of use with JoltPhysics APIs.
     * Memory ownership is managed by the PhysicsManager.
     */
    inline JPH::JobSystem *GetJobSystem() { return _jobSystem.get(); }

    /**
     * @brief Get the number of collision steps.
     *
     * @return int
     */
    inline int GetCollisionSteps() { return _collisionSteps; }

    /**
     * @brief Set the number of collision steps.
     *
     * @param steps
     * @note This will be used to set the number of steps in the physics system.
     *
     * @return void
     */
    inline void SetCollisionSteps(int steps) { _collisionSteps = steps; }

    /**
     * @brief Get the contact listener, casted back as a ContactListenerImpl.
     *
     * @return std::shared_ptr<Utils::ContactListenerImpl>
     *
     * @note This is safe because the contact listener is always created as a ContactListenerImpl.
     */
    inline std::shared_ptr<Utils::ContactListenerImpl> GetContactListener()
    {
        if (_contactListener == nullptr)
        {
            return nullptr;
        }

        return std::dynamic_pointer_cast<Utils::ContactListenerImpl>(_contactListener);
    }

    /**
     * @brief Add a contact added callback to the contact listener.
     *
     * @param callback The callback to add.
     *
     * @return void
     */
    inline void AddContactAddedCallback(Utils::ContactListenerImpl::OnContactAddedCallback callback)
    {
        auto contactListener = GetContactListener();

        if (contactListener != nullptr)
        {
            printf("Added a callback of smth\n");
            contactListener->AddOnContactAddedCallback(std::move(callback));
        }
#ifdef ES_DEBUG
        else
        {
            ES::Utils::Log::Error("PhysicsManager: tried to add contact added callback, but contact listener is not initialized.");
        }
#endif
    }

    /**
     * @brief Add a contact persisted callback to the contact listener.
     *
     * @param callback The callback to add.
     *
     * @return void
     */
    inline void AddContactPersistedCallback(Utils::ContactListenerImpl::OnContactPersistedCallback callback)
    {
        auto contactListener = GetContactListener();

        if (contactListener != nullptr)
        {
            contactListener->AddOnContactPersistedCallback(std::move(callback));
        }
#ifdef ES_DEBUG
        else
        {
            ES::Utils::Log::Error("PhysicsManager: tried to add contact persisted callback, but contact listener is not initialized.");
        }
#endif
    }

    /**
     * @brief Add a contact removed callback to the contact listener.
     *
     * @param callback The callback to add.
     *
     * @return void
     */
    inline void AddContactRemovedCallback(Utils::ContactListenerImpl::OnContactRemovedCallback callback)
    {
        auto contactListener = GetContactListener();

        if (contactListener != nullptr)
        {
            contactListener->AddOnContactRemovedCallback(std::move(callback));
        }
#ifdef ES_DEBUG
        else
        {
            ES::Utils::Log::Error("PhysicsManager: tried to add contact removed callback, but contact listener is not initialized.");
        }
#endif
    }

  private:
    std::shared_ptr<JPH::Factory> _factory;
    std::shared_ptr<JPH::PhysicsSystem> _physicsSystem;

    std::shared_ptr<JPH::BroadPhaseLayerInterface> _broadPhaseLayerInterface;
    std::shared_ptr<JPH::ObjectVsBroadPhaseLayerFilter> _objectVsBroadPhaseLayerFilter;
    std::shared_ptr<JPH::ObjectLayerPairFilter> _objectLayerPairFilter;
    std::shared_ptr<JPH::TempAllocator> _tempAllocator;
    std::shared_ptr<JPH::JobSystem> _jobSystem;
    std::shared_ptr<JPH::ContactListener> _contactListener;

    int _collisionSteps = 1;
};
} // namespace ES::Plugin::Physics::Resource
