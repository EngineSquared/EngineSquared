#pragma once

#include <memory>

#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

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
        ~PhysicsManager();

        /**
         * @brief Get a reference to the physics system.
         *
         * @return JPH::PhysicsSystem&
         */
        JPH::PhysicsSystem &GetPhysicsSystem() { return *_physicsSystem; }

        /**
         * @brief Get a pointer to the temp allocator.
         *
         * @return JPH::TempAllocator*
         * @note A raw pointer is returned for ease of use with JoltPhysics APIs.
         * Memory ownership is managed by the PhysicsManager.
         */
        JPH::TempAllocator *GetTempAllocator() { return _tempAllocator.get(); }

        /**
         * @brief Get a pointer to the job system.
         *
         * @return JPH::JobSystem*
         * @note A raw pointer is returned for ease of use with JoltPhysics APIs.
         * Memory ownership is managed by the PhysicsManager.
         */
        JPH::JobSystem *GetJobSystem() { return _jobSystem.get(); }

    private:
        std::shared_ptr<JPH::Factory> _factory;
        std::shared_ptr<JPH::PhysicsSystem> _physicsSystem;

        std::shared_ptr<JPH::BroadPhaseLayerInterface> _broadPhaseLayerInterface;
        std::shared_ptr<JPH::ObjectVsBroadPhaseLayerFilter> _objectVsBroadPhaseLayerFilter;
        std::shared_ptr<JPH::ObjectLayerPairFilter> _objectLayerPairFilter;
        std::shared_ptr<JPH::TempAllocator> _tempAllocator;
        std::shared_ptr<JPH::JobSystem> _jobSystem;
    };
}
