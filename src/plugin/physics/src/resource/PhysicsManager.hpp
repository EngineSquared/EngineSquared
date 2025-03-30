#pragma once

#include <memory>

// clang-format off
#include <Jolt/Jolt.h>
// clang-format on

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
     * @return void
     */
    void Init();

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

  private:
    std::shared_ptr<JPH::Factory> _factory;
    std::shared_ptr<JPH::PhysicsSystem> _physicsSystem;

    std::shared_ptr<JPH::BroadPhaseLayerInterface> _broadPhaseLayerInterface;
    std::shared_ptr<JPH::ObjectVsBroadPhaseLayerFilter> _objectVsBroadPhaseLayerFilter;
    std::shared_ptr<JPH::ObjectLayerPairFilter> _objectLayerPairFilter;
    std::shared_ptr<JPH::TempAllocator> _tempAllocator;
    std::shared_ptr<JPH::JobSystem> _jobSystem;

    int _collisionSteps;
};
} // namespace ES::Plugin::Physics::Resource
