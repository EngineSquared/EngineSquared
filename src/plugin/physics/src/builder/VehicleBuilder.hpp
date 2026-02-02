#pragma once

#include "component/BoxCollider.hpp"
#include "component/ConvexHullMeshCollider.hpp"
#include "component/MeshCollider.hpp"
#include "component/RigidBody.hpp"
#include "component/SoftBodyChassis.hpp"
#include "component/Vehicle.hpp"
#include "component/VehicleController.hpp"
#include "component/WheelSettings.hpp"
#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "exception/VehicleBuilderError.hpp"

#include "Object.hpp"

#include <array>
#include <fmt/format.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <memory>
#include <optional>

namespace Physics::Builder {

/**
 * @brief Builder for creating vehicles with N wheels
 *
 * Currently only supports N=4 (4-wheel vehicles).
 * Attempting to use other values will result in a compile-time error.
 *
 * Supports two chassis modes:
 * - RigidBody chassis (default): Standard rigid collision body
 * - SoftBody chassis: Deformable body that can be dented on collision
 *
 * Usage:
 * @code
 * VehicleBuilder<4> builder;
 * builder.SetChassisMesh(chassisMesh)
 *        .SetWheelMesh(WheelIndex::FrontLeft, wheelMesh)
 *        .SetWheelSettings(WheelIndex::FrontLeft, frontWheelSettings)
 *        .SetDrivetrain(DrivetrainType::RWD)
 *        .SetSoftBodyChassis(SoftBodyChassisSettings::Realistic())  // Optional
 *        .Build(core);
 * @endcode
 */
template <size_t N> class VehicleBuilder {
    static_assert(N == 4, "VehicleBuilder currently only supports 4-wheel vehicles (N=4). "
                          "Support for other wheel counts may be added in future versions.");
};

/**
 * @brief Specialized builder for 4-wheel vehicles
 */
template <> class VehicleBuilder<4> {
  public:
    /**
     * @brief Set the chassis mesh and initial transform
     *
     * @param chassisMesh Mesh for the vehicle body
     * @param position Initial position of the chassis
     * @param rotation Initial rotation of the chassis
     * @param scale Scale of the chassis mesh
     */
    VehicleBuilder &SetChassisMesh(const Object::Component::Mesh &chassisMesh,
                                   const glm::vec3 &position = glm::vec3(0.0f),
                                   const glm::quat &rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f),
                                   const glm::vec3 &scale = glm::vec3(1.0f))
    {
        _chassisMesh = chassisMesh;
        _chassisPosition = position;
        _chassisRotation = rotation;
        _chassisScale = scale;
        _hasChassisSet = true;
        return *this;
    }

    /**
     * @brief Set mesh for a specific wheel
     *
     * @param index Wheel position (FrontLeft, FrontRight, RearLeft, RearRight)
     * @param wheelMesh Mesh for the wheel visual
     */
    VehicleBuilder &SetWheelMesh(Component::WheelIndex index, const Object::Component::Mesh &wheelMesh)
    {
        _wheelMeshes[static_cast<size_t>(index)] = wheelMesh;
        _hasWheelMesh[static_cast<size_t>(index)] = true;
        return *this;
    }

    /**
     * @brief Set wheel settings for a specific wheel
     *
     * @param index Wheel position
     * @param settings Wheel physical properties
     */
    VehicleBuilder &SetWheelSettings(Component::WheelIndex index, const Component::WheelSettings &settings)
    {
        _vehicle.wheels[static_cast<size_t>(index)] = settings;
        return *this;
    }

    /**
     * @brief Set maximum brake torque for a specific wheel
     *
     * @param index Wheel position
     * @param torque Maximum brake torque in Nm (default: 1500.0f)
     */
    VehicleBuilder &SetMaxBrakeTorque(Component::WheelIndex index, float torque)
    {
        _vehicle.wheels[static_cast<size_t>(index)].maxBrakeTorque = torque;
        return *this;
    }

    /**
     * @brief Set maximum handbrake torque for a specific wheel
     *
     * Usually only applied to rear wheels.
     *
     * @param index Wheel position
     * @param torque Maximum handbrake torque in Nm (default: 4000.0f)
     */
    VehicleBuilder &SetMaxHandBrakeTorque(Component::WheelIndex index, float torque)
    {
        _vehicle.wheels[static_cast<size_t>(index)].maxHandBrakeTorque = torque;
        return *this;
    }

    /**
     * @brief Set drivetrain type (AWD, FWD, RWD)
     */
    VehicleBuilder &SetDrivetrain(Component::DrivetrainType drivetrain)
    {
        _vehicle.drivetrain = drivetrain;
        return *this;
    }

    /**
     * @brief Set engine configuration
     */
    VehicleBuilder &SetEngine(const Component::EngineSettings &engine)
    {
        _vehicle.engine = engine;
        return *this;
    }

    /**
     * @brief Set gearbox configuration
     */
    VehicleBuilder &SetGearbox(const Component::GearboxSettings &gearbox)
    {
        _vehicle.gearbox = gearbox;
        return *this;
    }

    /**
     * @brief Set rollbar configuration
     */
    VehicleBuilder &SetRollbar(const Component::RollbarSettings &rollbar)
    {
        _vehicle.rollbar = rollbar;
        return *this;
    }

    /**
     * @brief Enable soft body chassis for deformable car body
     *
     * When enabled, the vehicle will have a deformable chassis that can
     * be dented on collision. Uses a hybrid architecture with an invisible
     * RigidBody skeleton for physics and a visible SoftBody for deformation.
     *
     * @param settings SoftBody chassis configuration
     * @return Reference to builder for chaining
     *
     * @note If the mesh is too complex and cannot be simplified enough,
     *       the system will fall back to RigidBody chassis if fallbackToRigidBody
     *       is true in settings.
     */
    VehicleBuilder &SetSoftBodyChassis(const Component::SoftBodyChassisSettings &settings)
    {
        _softBodyChassisSettings = settings;
        _useSoftBodyChassis = true;
        return *this;
    }

    /**
     * @brief Enable soft body chassis with default realistic settings
     *
     * Convenience method that enables SoftBody chassis with settings
     * optimized for realistic deformation.
     *
     * @return Reference to builder for chaining
     */
    VehicleBuilder &EnableSoftBodyChassis()
    {
        _softBodyChassisSettings = Component::SoftBodyChassisSettings::Realistic();
        _useSoftBodyChassis = true;
        return *this;
    }

    /**
     * @brief Disable soft body chassis (use regular RigidBody)
     *
     * @return Reference to builder for chaining
     */
    VehicleBuilder &DisableSoftBodyChassis()
    {
        _useSoftBodyChassis = false;
        return *this;
    }

    /**
     * @brief Check if soft body chassis is enabled
     *
     * @return true if SoftBody chassis will be created
     */
    [[nodiscard]] bool IsSoftBodyChassisEnabled() const { return _useSoftBodyChassis; }

    /**
     * @brief Set wheel offset positions relative to chassis center
     *
     * Default positions assume a standard car layout:
     * - Front wheels: ±1.0 X, +1.5 Z
     * - Rear wheels: ±1.0 X, -1.5 Z
     * - All wheels: -0.3 Y (below chassis center)
     *
     * @param frontLeft Position of front-left wheel
     * @param frontRight Position of front-right wheel
     * @param rearLeft Position of rear-left wheel
     * @param rearRight Position of rear-right wheel
     */
    VehicleBuilder &SetWheelPositions(const glm::vec3 &frontLeft, const glm::vec3 &frontRight,
                                      const glm::vec3 &rearLeft, const glm::vec3 &rearRight)
    {
        using enum Component::WheelIndex;

        _wheelPositions[static_cast<size_t>(FrontLeft)] = frontLeft;
        _wheelPositions[static_cast<size_t>(FrontRight)] = frontRight;
        _wheelPositions[static_cast<size_t>(RearLeft)] = rearLeft;
        _wheelPositions[static_cast<size_t>(RearRight)] = rearRight;
        return *this;
    }

    /**
     * @brief Build the vehicle and create all entities
     *
     * Creates:
     * - 1 chassis entity with Vehicle, VehicleController, and Mesh components
     * - 4 wheel entities with Mesh and Transform components
     * - If SoftBody chassis enabled: adds SoftBodyChassis component
     *
     * The VehicleSystem will automatically create the Jolt constraint when
     * the Vehicle component is constructed.
     *
     * @param core Engine core reference
     * @return The chassis entity
     */
    Engine::Entity Build(Engine::Core &core)
    {
        if (!_hasChassisSet)
        {
            throw Exception::VehicleBuilderError("Chassis mesh not set");
        }

        for (size_t i = 0; i < 4; ++i)
        {
            if (!_hasWheelMesh[i])
            {
                throw Exception::VehicleBuilderError(fmt::format("Wheel mesh not set for wheel {}", i));
            }
        }

        if (_vehicle.gearbox.forwardGearRatios.empty())
        {
            throw Exception::VehicleBuilderError("Gearbox must have at least one forward gear");
        }

        auto chassis = core.CreateEntity();
        chassis.AddComponent<Object::Component::Transform>(
            Object::Component::Transform(_chassisPosition, _chassisScale, _chassisRotation));
        chassis.AddComponent<Object::Component::Mesh>(_chassisMesh);

        std::array<Engine::EntityId, 4> wheelEntities;
        for (size_t i = 0; i < 4; ++i)
        {
            wheelEntities[i] = core.CreateEntity();

            glm::vec3 worldWheelPos = _chassisPosition + _chassisRotation * _wheelPositions[i];
            glm::quat wheelRotation =
                _chassisRotation * glm::angleAxis(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

            wheelEntities[i].AddComponent<Object::Component::Transform>(
                core, Object::Component::Transform(worldWheelPos, glm::vec3(1.0f), wheelRotation));
            wheelEntities[i].AddComponent<Object::Component::Mesh>(core, _wheelMeshes[i]);
        }

        auto chassisRigidBody = Component::RigidBody::CreateDynamic(_chassisMass);
        chassisRigidBody.friction = 0.5f;
        chassisRigidBody.restitution = 0.1f;
        chassis.AddComponent<Component::ConvexHullMeshCollider>();
        chassis.AddComponent<Component::RigidBody>(chassisRigidBody);

        if (_useSoftBodyChassis)
        {
            chassis.AddComponent<Component::SoftBodyChassis>(_softBodyChassisSettings);
        }

        _vehicle.wheelEntities = wheelEntities;
        _vehicle.wheelPositions = _wheelPositions;

        chassis.AddComponent<Component::Vehicle>(_vehicle);

        chassis.AddComponent<Component::VehicleController>();

        return chassis;
    }

    /**
     * @brief Set chassis mass in kilograms
     */
    VehicleBuilder &SetChassisMass(float mass)
    {
        _chassisMass = mass;
        return *this;
    }

    /**
     * @brief Set the collision tester type for wheel-ground detection
     */
    VehicleBuilder &SetCollisionTesterType(Component::CollisionTesterType type)
    {
        _vehicle.collisionTesterType = type;
        return *this;
    }

    /**
     * @brief Set the convex radius fraction for CastCylinder tester
     *
     * @param fraction Value between 0.0 and 1.0 (default: 0.5)
     */
    VehicleBuilder &SetConvexRadiusFraction(float fraction)
    {
        if (fraction < 0.0f || fraction > 1.0f)
        {
            throw Exception::VehicleBuilderError("Convex radius fraction must be between 0.0 and 1.0");
        }
        _vehicle.convexRadiusFraction = fraction;
        return *this;
    }

  private:
    Component::Vehicle _vehicle = Component::Vehicle::CreateDefaultCar();

    Object::Component::Mesh _chassisMesh;
    glm::vec3 _chassisPosition = glm::vec3(0.0f);
    glm::quat _chassisRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 _chassisScale = glm::vec3(1.0f);
    float _chassisMass = 1500.0f;

    std::array<Object::Component::Mesh, 4> _wheelMeshes;
    std::array<glm::vec3, 4> _wheelPositions = Component::Vehicle::GetDefaultWheelPositions();

    bool _hasChassisSet = false;
    std::array<bool, 4> _hasWheelMesh = {false, false, false, false};

    // SoftBody chassis settings
    Component::SoftBodyChassisSettings _softBodyChassisSettings;
    bool _useSoftBodyChassis = false;
};

} // namespace Physics::Builder
