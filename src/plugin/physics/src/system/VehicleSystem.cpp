#include "Physics.pch.hpp"

#include "system/VehicleSystem.hpp"

#include "Logger.hpp"
#include "Object.hpp"
#include "component/RigidBody.hpp"
#include "component/RigidBodyInternal.hpp"
#include "component/Vehicle.hpp"
#include "component/VehicleInternal.hpp"
#include "resource/PhysicsManager.hpp"
#include "utils/JoltConversions.hpp"

#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Vehicle/WheeledVehicleController.h>

#include <array>
#include <fmt/format.h>
#include <glm/glm.hpp>

namespace Physics::System {

/**
 * @brief Create Jolt wheel settings from Physics wheel configuration
 */
static void CreateJoltWheelSettings(JPH::WheelSettingsWV &joltWheel, const Component::WheelSettings &wheelSettings,
                                    const glm::vec3 &position, bool isRear)
{
    joltWheel.mPosition = Utils::ToJoltVec3(position);
    joltWheel.mSuspensionDirection = JPH::Vec3(0, -1, 0);
    joltWheel.mSteeringAxis = JPH::Vec3(0, 1, 0);
    joltWheel.mWheelUp = JPH::Vec3(0, 1, 0);
    joltWheel.mWheelForward = JPH::Vec3(0, 0, 1);
    joltWheel.mSuspensionMinLength = wheelSettings.suspensionMinLength;
    joltWheel.mSuspensionMaxLength = wheelSettings.suspensionMaxLength;
    joltWheel.mSuspensionPreloadLength = 0.0f;
    joltWheel.mMaxSteerAngle = wheelSettings.maxSteerAngle;
    joltWheel.mMaxHandBrakeTorque = isRear ? 1500.0f : 0.0f;
    joltWheel.mRadius = wheelSettings.radius;
    joltWheel.mWidth = wheelSettings.width;

    joltWheel.mSuspensionSpring.mFrequency = wheelSettings.suspensionFrequency;
    joltWheel.mSuspensionSpring.mDamping = wheelSettings.suspensionDamping;

    // Convert friction curves
    joltWheel.mInertia = wheelSettings.inertia;
    joltWheel.mAngularDamping = wheelSettings.angularDamping;
    joltWheel.mMaxBrakeTorque = wheelSettings.maxBrakeTorque;
    joltWheel.mMaxHandBrakeTorque = wheelSettings.maxHandBrakeTorque;

    // Convert longitudinal friction curve
    joltWheel.mLongitudinalFriction.Clear();
    joltWheel.mLongitudinalFriction.Reserve(static_cast<JPH::uint>(wheelSettings.longitudinalFriction.size()));
    for (const auto &point : wheelSettings.longitudinalFriction)
    {
        joltWheel.mLongitudinalFriction.AddPoint(point.slip, point.friction);
    }

    // Convert lateral friction curve
    joltWheel.mLateralFriction.Clear();
    joltWheel.mLateralFriction.Reserve(static_cast<JPH::uint>(wheelSettings.lateralFriction.size()));
    for (const auto &point : wheelSettings.lateralFriction)
    {
        joltWheel.mLateralFriction.AddPoint(point.slip, point.friction);
    }
}

/**
 * @brief Called when Vehicle component is added to an entity
 *
 * Creates:
 * 1. Jolt WheeledVehicleController with all settings
 * 2. VehicleConstraint attached to chassis
 * 3. VehicleInternal component with constraint data
 *
 * Note: Jolt's WheeledVehicle does NOT use separate physics bodies for wheels.
 * Wheels are simulated via raycasts/shape casts from the chassis.
 */
static void OnVehicleConstruct(Engine::Core::Registry &registry, Engine::EntityId entityId)
{
    auto &core = registry.ctx().get<Engine::Core *>();
    auto entity = Engine::Entity{*core, entityId};
    auto &physicsManager = core->GetResource<Resource::PhysicsManager>();

    if (!physicsManager.IsPhysicsActivated())
    {
        Log::Error("Cannot create Vehicle: Physics system not activated");
        return;
    }

    auto &vehicle = entity.GetComponents<Component::Vehicle>();

    auto *chassisInternal = entity.TryGetComponent<Component::RigidBodyInternal>();
    if (!chassisInternal || !chassisInternal->IsValid())
    {
        Log::Error("Cannot create Vehicle: Chassis must have a valid RigidBody component");
        return;
    }

    std::array<Engine::EntityId, 4> wheelEntities = vehicle.wheelEntities;
    std::array<JPH::BodyID, 4> wheelBodyIDs{};

    JPH::WheeledVehicleControllerSettings controllerSettings;

    controllerSettings.mEngine.mMaxTorque = vehicle.engine.maxTorque;
    controllerSettings.mEngine.mMinRPM = vehicle.engine.minRPM;
    controllerSettings.mEngine.mMaxRPM = vehicle.engine.maxRPM;
    controllerSettings.mEngine.mInertia = vehicle.engine.inertia;
    controllerSettings.mEngine.mAngularDamping = vehicle.engine.angularDamping;

    // Convert normalized torque curve
    controllerSettings.mEngine.mNormalizedTorque.Clear();
    controllerSettings.mEngine.mNormalizedTorque.Reserve(
        static_cast<JPH::uint>(vehicle.engine.normalizedTorque.size()));
    for (const auto &point : vehicle.engine.normalizedTorque)
    {
        controllerSettings.mEngine.mNormalizedTorque.AddPoint(point.rpm, point.torque);
    }

    // Convert transmission mode
    controllerSettings.mTransmission.mMode = (vehicle.gearbox.mode == Component::TransmissionMode::Auto) ?
                                                 JPH::ETransmissionMode::Auto :
                                                 JPH::ETransmissionMode::Manual;

    controllerSettings.mTransmission.mClutchStrength = vehicle.gearbox.clutchStrength;
    controllerSettings.mTransmission.mSwitchTime = vehicle.gearbox.switchTime;
    controllerSettings.mTransmission.mClutchReleaseTime = vehicle.gearbox.clutchReleaseTime;
    controllerSettings.mTransmission.mSwitchLatency = vehicle.gearbox.switchLatency;
    controllerSettings.mTransmission.mShiftUpRPM = vehicle.gearbox.shiftUpRPM;
    controllerSettings.mTransmission.mShiftDownRPM = vehicle.gearbox.shiftDownRPM;
    controllerSettings.mTransmission.mGearRatios.clear();

    if (vehicle.gearbox.forwardGearRatios.empty())
    {
        Log::Error("Cannot create Vehicle: Gearbox must have at least one forward gear");
        return;
    }

    // Convert forward gear ratios (std::vector<float> -> JPH::Array<float>)
    for (float ratio : vehicle.gearbox.forwardGearRatios)
    {
        controllerSettings.mTransmission.mGearRatios.push_back(ratio);
    }

    // Convert reverse gear ratios (std::vector<float> -> JPH::Array<float>)
    controllerSettings.mTransmission.mReverseGearRatios.clear();
    for (float ratio : vehicle.gearbox.reverseGearRatios)
    {
        controllerSettings.mTransmission.mReverseGearRatios.push_back(ratio);
    }

    switch (vehicle.drivetrain)
    {
    case Component::DrivetrainType::FWD:
        controllerSettings.mDifferentials.resize(1);
        controllerSettings.mDifferentials[0].mLeftWheel = 0;
        controllerSettings.mDifferentials[0].mRightWheel = 1;
        controllerSettings.mDifferentials[0].mDifferentialRatio = 3.42f;
        controllerSettings.mDifferentials[0].mLeftRightSplit = 0.5f;
        controllerSettings.mDifferentials[0].mLimitedSlipRatio = 1.4f;
        controllerSettings.mDifferentials[0].mEngineTorqueRatio = 1.0f;
        break;

    case Component::DrivetrainType::RWD:
        controllerSettings.mDifferentials.resize(1);
        controllerSettings.mDifferentials[0].mLeftWheel = 2;
        controllerSettings.mDifferentials[0].mRightWheel = 3;
        controllerSettings.mDifferentials[0].mDifferentialRatio = 3.42f;
        controllerSettings.mDifferentials[0].mLeftRightSplit = 0.5f;
        controllerSettings.mDifferentials[0].mLimitedSlipRatio = 1.4f;
        controllerSettings.mDifferentials[0].mEngineTorqueRatio = 1.0f;
        break;

    case Component::DrivetrainType::AWD:
        controllerSettings.mDifferentials.resize(3);
        controllerSettings.mDifferentials[0].mLeftWheel = 0;
        controllerSettings.mDifferentials[0].mRightWheel = 1;
        controllerSettings.mDifferentials[0].mDifferentialRatio = 3.42f;
        controllerSettings.mDifferentials[0].mLeftRightSplit = 0.5f;
        controllerSettings.mDifferentials[0].mLimitedSlipRatio = 1.4f;

        controllerSettings.mDifferentials[1].mLeftWheel = 2;
        controllerSettings.mDifferentials[1].mRightWheel = 3;
        controllerSettings.mDifferentials[1].mDifferentialRatio = 3.42f;
        controllerSettings.mDifferentials[1].mLeftRightSplit = 0.5f;
        controllerSettings.mDifferentials[1].mLimitedSlipRatio = 1.4f;

        controllerSettings.mDifferentials[2].mLeftWheel = -1;
        controllerSettings.mDifferentials[2].mRightWheel = -2;
        controllerSettings.mDifferentials[2].mDifferentialRatio = 1.0f;
        controllerSettings.mDifferentials[2].mLeftRightSplit = 0.5f;
        controllerSettings.mDifferentials[2].mLimitedSlipRatio = 1.4f;
        controllerSettings.mDifferentials[2].mEngineTorqueRatio = 1.0f;
        break;
    }

    JPH::VehicleConstraintSettings constraintSettings;

    const std::array<glm::vec3, 4> &wheelPositions = vehicle.wheelPositions;

    constraintSettings.mWheels.resize(4);
    for (size_t i = 0; i < 4; ++i)
    {
        bool isRear = (i >= 2);
        auto *wheelSettings = new JPH::WheelSettingsWV();
        CreateJoltWheelSettings(*wheelSettings, vehicle.wheels[i], wheelPositions[i], isRear);
        constraintSettings.mWheels[i] = wheelSettings;
    }

    constraintSettings.mController = new JPH::WheeledVehicleControllerSettings(controllerSettings);
    constraintSettings.mMaxPitchRollAngle = JPH::DegreesToRadians(60.0f);

    constraintSettings.mAntiRollBars.clear();
    if (vehicle.rollbar.frontStiffness > 0.0f)
    {
        JPH::VehicleAntiRollBar frontBar;
        frontBar.mLeftWheel = 0;
        frontBar.mRightWheel = 1;
        frontBar.mStiffness = vehicle.rollbar.frontStiffness;
        constraintSettings.mAntiRollBars.push_back(frontBar);
    }
    if (vehicle.rollbar.rearStiffness > 0.0f)
    {
        JPH::VehicleAntiRollBar rearBar;
        rearBar.mLeftWheel = 2;
        rearBar.mRightWheel = 3;
        rearBar.mStiffness = vehicle.rollbar.rearStiffness;
        constraintSettings.mAntiRollBars.push_back(rearBar);
    }

    JPH::Ref<JPH::VehicleConstraint> vehicleConstraint;
    {
        JPH::BodyLockWrite lock(physicsManager.GetPhysicsSystem().GetBodyLockInterface(), chassisInternal->bodyID);
        if (lock.Succeeded())
        {
            JPH::Body &chassisBody = lock.GetBody();
            vehicleConstraint = new JPH::VehicleConstraint(chassisBody, constraintSettings);
        }
        else
        {
            Log::Error("Failed to lock chassis body for vehicle constraint creation");
            return;
        }
    }

    JPH::Ref<JPH::VehicleCollisionTester> collisionTester;

    {
        using enum Component::CollisionTesterType;
        switch (vehicle.collisionTesterType)
        {
        case Ray: collisionTester = new JPH::VehicleCollisionTesterRay(Utils::Layers::MOVING); break;
        case CastSphere:
            collisionTester =
                new JPH::VehicleCollisionTesterCastSphere(Utils::Layers::MOVING, 0.5f * vehicle.wheels[0].width);
            break;
        case CastCylinder:
        default:
            collisionTester =
                new JPH::VehicleCollisionTesterCastCylinder(Utils::Layers::MOVING, vehicle.convexRadiusFraction);
            break;
        }
    }

    vehicleConstraint->SetVehicleCollisionTester(collisionTester);

    physicsManager.GetPhysicsSystem().AddConstraint(vehicleConstraint);
    physicsManager.GetPhysicsSystem().AddStepListener(vehicleConstraint);

    auto &vehicleInternal = entity.AddComponent<Component::VehicleInternal>();
    vehicleInternal.vehicleConstraint = vehicleConstraint;
    vehicleInternal.collisionTester = collisionTester;
    vehicleInternal.wheelEntities = wheelEntities;
    vehicleInternal.wheelBodyIDs = wheelBodyIDs;
    vehicleInternal.chassisBodyID = chassisInternal->bodyID;

    Log::Debug(fmt::format("Created Vehicle for entity {}", entity));
}

/**
 * @brief Called when Vehicle component is removed from an entity
 *
 * Destroys:
 * 1. Jolt VehicleConstraint
 * 2. VehicleInternal component
 */
static void OnVehicleDestroy(Engine::Core::Registry &registry, Engine::EntityId entityId)
{
    auto &core = registry.ctx().get<Engine::Core *>();
    auto &physicsManager = core->GetResource<Resource::PhysicsManager>();
    auto entity = Engine::Entity{*core, entityId};

    if (!physicsManager.IsPhysicsActivated())
        return;

    auto *vehicleInternal = entity.TryGetComponent<Component::VehicleInternal>();
    if (!vehicleInternal || !vehicleInternal->IsValid())
        return;

    physicsManager.GetPhysicsSystem().RemoveStepListener(vehicleInternal->vehicleConstraint);
    physicsManager.GetPhysicsSystem().RemoveConstraint(vehicleInternal->vehicleConstraint);

    vehicleInternal->vehicleConstraint = nullptr;

    Log::Debug(fmt::format("Destroyed Vehicle for entity {}", entity));
    entity.RemoveComponent<Component::VehicleInternal>();
}

void InitVehicleSystem(Engine::Core &core)
{
    auto &registry = core.GetRegistry();

    registry.on_construct<Component::Vehicle>().connect<&OnVehicleConstruct>();
    registry.on_destroy<Component::Vehicle>().connect<&OnVehicleDestroy>();
}

} // namespace Physics::System
