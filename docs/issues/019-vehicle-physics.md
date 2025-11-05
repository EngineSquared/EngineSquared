# Issue #019: Implement Vehicle Physics (Wheels, Suspension, Engine)

**Milestone:** v0.7 - Advanced Physics  
**Priority:** 🟡 HIGH  
**Estimated Effort:** 6-7 days  
**Dependencies:** #001-005 (Core Physics), #015-017 (Constraints for suspension/wheels)  
**Related Issues:** #018 (Character Controller), #020 (Ragdoll System)  
**Follow-up Issues:** #024 (Debug Visualization), #026 (Editor Integration)

---

## 📋 Description

Implement physics-based vehicle system with realistic wheel physics, suspension, engine, steering, and braking. Supports cars, trucks, and other wheeled vehicles with configurable parameters.

## 🎯 Goals

- ✅ Implement Vehicle component (chassis)
- ✅ Implement Wheel component (individual wheels)
- ✅ Suspension system (spring-damper)
- ✅ Engine simulation (torque curve)
- ✅ Steering system (Ackermann)
- ✅ Braking system (front/rear bias)
- ✅ Wheel friction (grip, slip)
- ✅ Full test coverage
- ✅ Documentation and examples

## 🔧 Technical Details

### Files to Create/Modify

```
src/plugin/physics/src/
├── component/
│   ├── Vehicle.hpp                 # New file
│   └── Wheel.hpp                   # New file
├── resource/
│   ├── VehicleController.hpp       # New file
│   └── VehicleController.cpp       # New file
├── system/
│   ├── VehicleSystem.hpp           # New file
│   └── VehicleSystem.cpp           # New file
└── tests/
    └── VehicleTest.cpp             # New file
```

### Vehicle Component

```cpp
/**************************************************************************
 * @file Vehicle.hpp
 * @brief Vehicle component (chassis with wheels)
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Engine {
    class Entity;
}

namespace Physics::Component {

/**
 * @brief Engine settings
 */
struct EngineSettings {
    float maxTorque = 300.0f;          ///< Max engine torque (Nm)
    float maxRPM = 6000.0f;            ///< Redline RPM
    float idleRPM = 800.0f;            ///< Idle RPM
    float inertia = 0.5f;              ///< Engine inertia
    
    /**
     * @brief Get torque at RPM (simple torque curve)
     * @param rpm Current RPM
     * @return Torque (Nm)
     */
    float GetTorque(float rpm) const {
        if (rpm < idleRPM) return 0.0f;
        if (rpm > maxRPM) return 0.0f;
        
        // Peak torque at 60% of max RPM
        float peakRPM = maxRPM * 0.6f;
        float t = rpm / peakRPM;
        
        if (rpm < peakRPM) {
            // Linear rise to peak
            return maxTorque * t;
        } else {
            // Falloff after peak
            float falloff = (maxRPM - rpm) / (maxRPM - peakRPM);
            return maxTorque * falloff;
        }
    }
};

/**
 * @brief Differential type
 */
enum class DifferentialType {
    Open,         ///< Open differential (simple)
    Locked,       ///< Locked differential (equal torque)
    LimitedSlip   ///< Limited slip differential (balanced)
};

/**
 * @brief Vehicle settings
 */
struct VehicleSettings {
    /// Engine
    EngineSettings engine;
    
    /// Transmission
    std::vector<float> gearRatios = {3.5f, 2.5f, 1.8f, 1.3f, 1.0f, 0.8f};  ///< Gear ratios (1st to 6th)
    float finalDriveRatio = 3.5f;      ///< Final drive ratio
    float reverseGearRatio = 3.0f;     ///< Reverse gear ratio
    
    /// Differential
    DifferentialType differentialType = DifferentialType::Open;
    float differentialRatio = 0.5f;    ///< Torque split (0 = front, 1 = rear, 0.5 = even)
    
    /// Steering
    float maxSteerAngle = 35.0f;       ///< Max steering angle (degrees)
    float steerSpeed = 3.0f;           ///< Steering speed (rad/s)
    bool ackermannSteering = true;     ///< Use Ackermann steering geometry
    
    /// Braking
    float maxBrakeTorque = 1500.0f;    ///< Max brake torque (Nm)
    float brakeBias = 0.6f;            ///< Brake bias (0 = rear, 1 = front, 0.6 = 60% front)
    float handBrakeTorque = 2000.0f;   ///< Handbrake torque (Nm, rear only)
    
    /// Downforce
    float downforceCoefficient = 0.0f; ///< Downforce coefficient (0 = none)
    
    /**
     * @brief Create default car settings
     * @return VehicleSettings
     */
    static VehicleSettings DefaultCar() {
        return {};
    }
    
    /**
     * @brief Create sports car settings (high power, grip)
     * @return VehicleSettings
     */
    static VehicleSettings SportsCar() {
        VehicleSettings s;
        s.engine.maxTorque = 500.0f;
        s.engine.maxRPM = 8000.0f;
        s.maxSteerAngle = 30.0f;
        s.downforceCoefficient = 0.5f;
        return s;
    }
    
    /**
     * @brief Create truck settings (high torque, slow)
     * @return VehicleSettings
     */
    static VehicleSettings Truck() {
        VehicleSettings s;
        s.engine.maxTorque = 800.0f;
        s.engine.maxRPM = 3000.0f;
        s.maxSteerAngle = 40.0f;
        s.gearRatios = {5.0f, 3.5f, 2.5f, 1.8f, 1.3f, 1.0f};
        return s;
    }
};

/**
 * @brief Vehicle component (chassis)
 * 
 * Represents a wheeled vehicle with engine, transmission, and control systems.
 * Must have Wheel components attached to child entities.
 * 
 * @example
 * @code
 * // Create car chassis
 * auto car = core.CreateEntity();
 * car.AddComponent<Object::Component::Transform>(core,
 *     Object::Component::Transform::Create(glm::vec3(0, 1, 0)));
 * 
 * auto rb = Physics::Component::RigidBody::CreateDynamic(1500.0f);  // 1.5 tons
 * rb.centerOfMass = glm::vec3(0, -0.3f, 0);  // Lower CoM for stability
 * car.AddComponent<Physics::Component::RigidBody>(core, rb);
 * 
 * Physics::Component::BoxCollider collider(glm::vec3(2, 0.5f, 4));
 * car.AddComponent<Physics::Component::BoxCollider>(core, collider);
 * 
 * auto vehicle = Physics::Component::Vehicle::Create(
 *     Physics::Component::VehicleSettings::DefaultCar()
 * );
 * car.AddComponent<Physics::Component::Vehicle>(core, vehicle);
 * 
 * // Add wheels (see Wheel component)
 * @endcode
 */
struct Vehicle {
    /// Settings
    VehicleSettings settings;
    
    /// Wheels (entity IDs of child wheels)
    std::vector<Engine::Entity> wheels;
    
    /// State (read-only, updated by system)
    float throttle = 0.0f;        ///< Current throttle [0, 1]
    float brake = 0.0f;           ///< Current brake [0, 1]
    float steerInput = 0.0f;      ///< Current steering [-1, 1]
    bool handBrake = false;       ///< Handbrake engaged
    
    int currentGear = 1;          ///< Current gear (0 = neutral, -1 = reverse)
    float engineRPM = 800.0f;     ///< Current engine RPM
    float speed = 0.0f;           ///< Current speed (m/s)
    
    /// Internal (Jolt)
    uint32_t joltVehicleId = 0;
    
    /**
     * @brief Create vehicle
     * @param vehicleSettings Settings
     * @return Vehicle
     */
    static Vehicle Create(const VehicleSettings &vehicleSettings = VehicleSettings::DefaultCar()) {
        Vehicle v;
        v.settings = vehicleSettings;
        return v;
    }
    
    /**
     * @brief Add wheel to vehicle
     * @param wheel Wheel entity
     */
    void AddWheel(Engine::Entity wheel) {
        wheels.push_back(wheel);
    }
    
    /**
     * @brief Get speed in km/h
     * @return Speed (km/h)
     */
    float GetSpeedKmh() const {
        return speed * 3.6f;
    }
    
    /**
     * @brief Get speed in mph
     * @return Speed (mph)
     */
    float GetSpeedMph() const {
        return speed * 2.237f;
    }
    
    /**
     * @brief Check if in reverse gear
     * @return true if reverse
     */
    bool IsReverse() const {
        return currentGear < 0;
    }
};

} // namespace Physics::Component
```

### Wheel Component

```cpp
/**************************************************************************
 * @file Wheel.hpp
 * @brief Wheel component (individual wheel)
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>

namespace Engine {
    class Entity;
}

namespace Physics::Component {

/**
 * @brief Wheel position
 */
enum class WheelPosition {
    FrontLeft,
    FrontRight,
    RearLeft,
    RearRight,
    Other
};

/**
 * @brief Wheel settings
 */
struct WheelSettings {
    /// Geometry
    float radius = 0.35f;          ///< Wheel radius (meters)
    float width = 0.2f;            ///< Wheel width (meters)
    float mass = 20.0f;            ///< Wheel mass (kg)
    float inertia = 0.5f;          ///< Rotational inertia
    
    /// Suspension
    float suspensionLength = 0.3f;  ///< Suspension travel (meters)
    float suspensionStiffness = 50000.0f;  ///< Spring stiffness (N/m)
    float suspensionDamping = 3000.0f;     ///< Damping (Ns/m)
    float suspensionMaxTravel = 0.3f;      ///< Max compression (meters)
    
    /// Friction
    float lateralFriction = 1.5f;   ///< Lateral grip coefficient
    float longitudinalFriction = 1.2f; ///< Longitudinal grip coefficient
    float rollingResistance = 0.015f;  ///< Rolling resistance coefficient
    
    /// Wheel drive
    bool isDriven = false;         ///< Receives engine torque
    bool isSteered = false;        ///< Affected by steering
    
    /**
     * @brief Create default wheel
     * @return WheelSettings
     */
    static WheelSettings Default() {
        return {};
    }
    
    /**
     * @brief Create front wheel (steered, driven for FWD/AWD)
     * @param driven Whether driven (FWD/AWD)
     * @return WheelSettings
     */
    static WheelSettings Front(bool driven = false) {
        WheelSettings s;
        s.isSteered = true;
        s.isDriven = driven;
        return s;
    }
    
    /**
     * @brief Create rear wheel (driven for RWD/AWD)
     * @param driven Whether driven (RWD/AWD)
     * @return WheelSettings
     */
    static WheelSettings Rear(bool driven = true) {
        WheelSettings s;
        s.isDriven = driven;
        return s;
    }
};

/**
 * @brief Wheel component
 * 
 * Represents a single wheel of a vehicle. Must be attached to
 * an entity that is a child of a Vehicle entity.
 * 
 * @example
 * @code
 * // Create wheel (child of vehicle)
 * auto wheel = core.CreateEntity();
 * wheel.AddComponent<Object::Component::Transform>(core,
 *     Object::Component::Transform::Create(glm::vec3(1, 0, 1.5f)));  // Front right
 * 
 * auto wheelComp = Physics::Component::Wheel::Create(
 *     Physics::Component::WheelSettings::Front(true),  // Front wheel, driven (AWD)
 *     Physics::Component::WheelPosition::FrontRight
 * );
 * wheel.AddComponent<Physics::Component::Wheel>(core, wheelComp);
 * 
 * // Add to vehicle
 * vehicle.AddWheel(wheel);
 * @endcode
 */
struct Wheel {
    /// Settings
    WheelSettings settings;
    
    /// Position
    WheelPosition position = WheelPosition::Other;
    Engine::Entity vehicle;        ///< Parent vehicle entity
    
    /// State (read-only, updated by system)
    float angularVelocity = 0.0f;  ///< Wheel angular velocity (rad/s)
    float steerAngle = 0.0f;       ///< Current steer angle (radians)
    float suspensionCompression = 0.0f;  ///< Current compression [0, 1]
    float brakeForce = 0.0f;       ///< Current brake force (Nm)
    float motorTorque = 0.0f;      ///< Current motor torque (Nm)
    bool isGrounded = false;       ///< Wheel touching ground
    glm::vec3 groundNormal = glm::vec3(0, 1, 0);  ///< Ground normal
    
    /// Slip (for effects)
    float lateralSlip = 0.0f;      ///< Lateral slip [-1, 1]
    float longitudinalSlip = 0.0f; ///< Longitudinal slip [-1, 1]
    
    /// Internal (Jolt)
    uint32_t joltWheelId = 0;
    
    /**
     * @brief Create wheel
     * @param wheelSettings Settings
     * @param pos Wheel position
     * @return Wheel
     */
    static Wheel Create(
        const WheelSettings &wheelSettings = WheelSettings::Default(),
        WheelPosition pos = WheelPosition::Other
    ) {
        Wheel w;
        w.settings = wheelSettings;
        w.position = pos;
        return w;
    }
    
    /**
     * @brief Get wheel RPM
     * @return RPM
     */
    float GetRPM() const {
        return angularVelocity * 60.0f / (2.0f * glm::pi<float>());
    }
    
    /**
     * @brief Check if slipping
     * @return true if slipping
     */
    bool IsSlipping() const {
        return glm::abs(lateralSlip) > 0.3f || glm::abs(longitudinalSlip) > 0.3f;
    }
};

} // namespace Physics::Component
```

### VehicleController Resource

```cpp
/**************************************************************************
 * @file VehicleController.hpp
 * @brief Vehicle control utilities
 **************************************************************************/

#pragma once

namespace Engine {
    class Core;
    class Entity;
}

namespace Physics::Resource {

/**
 * @brief Vehicle control utilities
 * 
 * Provides functions to control vehicles based on input.
 * Works with Input plugin for keyboard/gamepad control.
 */
class VehicleController {
public:
    /**
     * @brief Set throttle input
     * @param core Engine core
     * @param entity Vehicle entity
     * @param throttle Throttle [0, 1]
     */
    static void SetThrottle(Engine::Core &core, Engine::Entity entity, float throttle);
    
    /**
     * @brief Set brake input
     * @param core Engine core
     * @param entity Vehicle entity
     * @param brake Brake [0, 1]
     */
    static void SetBrake(Engine::Core &core, Engine::Entity entity, float brake);
    
    /**
     * @brief Set steering input
     * @param core Engine core
     * @param entity Vehicle entity
     * @param steer Steering [-1, 1] (left/right)
     */
    static void SetSteering(Engine::Core &core, Engine::Entity entity, float steer);
    
    /**
     * @brief Set handbrake
     * @param core Engine core
     * @param entity Vehicle entity
     * @param engaged Handbrake engaged
     */
    static void SetHandBrake(Engine::Core &core, Engine::Entity entity, bool engaged);
    
    /**
     * @brief Shift gear
     * @param core Engine core
     * @param entity Vehicle entity
     * @param gear Target gear (0 = neutral, -1 = reverse, 1+ = forward)
     */
    static void ShiftGear(Engine::Core &core, Engine::Entity entity, int gear);
    
    /**
     * @brief Shift up
     * @param core Engine core
     * @param entity Vehicle entity
     */
    static void ShiftUp(Engine::Core &core, Engine::Entity entity);
    
    /**
     * @brief Shift down
     * @param core Engine core
     * @param entity Vehicle entity
     */
    static void ShiftDown(Engine::Core &core, Engine::Entity entity);
    
    /**
     * @brief Reset vehicle (flip upright)
     * @param core Engine core
     * @param entity Vehicle entity
     */
    static void ResetVehicle(Engine::Core &core, Engine::Entity entity);
};

} // namespace Physics::Resource
```

## 📝 Implementation Tasks

### Phase 1: Create Components (4-5 hours)

- [ ] Create `component/Vehicle.hpp`
- [ ] Implement EngineSettings (torque curve)
- [ ] Implement DifferentialType enum
- [ ] Implement VehicleSettings
- [ ] Implement Vehicle component
- [ ] Create `component/Wheel.hpp`
- [ ] Implement WheelPosition enum
- [ ] Implement WheelSettings
- [ ] Implement Wheel component
- [ ] Add Doxygen comments
- [ ] Add to `Physics.hpp`

### Phase 2: Implement VehicleController (12-14 hours)

- [ ] Create `resource/VehicleController.hpp`
- [ ] Implement SetThrottle()
  - Calculate engine torque from RPM
  - Apply gear ratio
  - Distribute to driven wheels
- [ ] Implement SetBrake()
  - Calculate brake torque
  - Apply brake bias (front/rear)
- [ ] Implement SetSteering()
  - Calculate steer angle
  - Apply Ackermann geometry
- [ ] Implement SetHandBrake()
- [ ] Implement ShiftGear/ShiftUp/ShiftDown
- [ ] Implement ResetVehicle

### Phase 3: Implement VehicleSystem (14-16 hours)

- [ ] Create `system/VehicleSystem.hpp`
- [ ] Implement Initialize() / Shutdown()
- [ ] Implement Update()
  - Update engine RPM
  - Apply motor torque to wheels
  - Apply brake torque
  - Update steering
  - Update suspension compression
  - Calculate wheel slip
  - Apply downforce
- [ ] Integrate with Jolt Vehicle
- [ ] Handle wheel ground detection

### Phase 4: Unit Tests (10-12 hours)

- [ ] Create `tests/VehicleTest.cpp`
- [ ] Test throttle (engine torque)
- [ ] Test braking (brake bias)
- [ ] Test steering (Ackermann)
- [ ] Test suspension (compression)
- [ ] Test gear shifting
- [ ] Test wheel slip detection
- [ ] Test differential (torque split)

### Phase 5: Integration Examples (12-14 hours)

- [ ] Update `examples/physics_usage/src/main.cpp`
- [ ] Add "Basic Car" example (4-wheel, RWD)
- [ ] Add "Sports Car" example (high power, AWD)
- [ ] Add "Truck" example (heavy, slow)
- [ ] Add "Drift Car" example (high slip)
- [ ] Add "Suspension Test" example (bumpy terrain)
- [ ] Add "Vehicle Reset" example (flip recovery)

### Phase 6: Documentation (5-6 hours)

- [ ] Update `RIGIDBODY_USAGE.md`
  - Add "Vehicle Physics" section
  - Explain wheel setup
  - Explain suspension tuning
  - Explain engine/transmission
  - Integration with Input plugin
- [ ] Add practical examples

## 🧪 Testing Checklist

### Unit Tests

- [ ] Throttle applies torque to wheels
- [ ] Brake applies braking force
- [ ] Steering rotates wheels (Ackermann)
- [ ] Suspension compresses/extends
- [ ] Gear shifting works
- [ ] Wheel slip detection works
- [ ] Differential splits torque
- [ ] Handbrake locks rear wheels

### Integration Tests

- [ ] Vehicle accelerates forward
- [ ] Vehicle brakes to stop
- [ ] Vehicle steers left/right
- [ ] Vehicle climbs slopes
- [ ] Suspension absorbs bumps
- [ ] Vehicle drifts on low friction
- [ ] Reset flips vehicle upright

### Performance Tests

- [ ] 10 vehicles: < 5ms overhead
- [ ] Wheel physics: < 0.5ms per wheel

### Edge Cases

- [ ] Very high throttle (wheel spin)
- [ ] Sudden braking (lockup)
- [ ] Steep slope (rollback)
- [ ] Flip vehicle (reset)

## 📚 Code Examples

### Example 1: Create Basic Car (4-wheel RWD)

```cpp
// Create simple car with 4 wheels (rear-wheel drive)
Engine::Entity CreateCar(Engine::Core &core, glm::vec3 spawnPos) {
    // Chassis
    auto car = core.CreateEntity();
    car.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(spawnPos));
    
    // Physics (1500kg car)
    auto rb = Physics::Component::RigidBody::CreateDynamic(1500.0f);
    rb.centerOfMass = glm::vec3(0, -0.3f, 0);  // Lower CoM for stability
    car.AddComponent<Physics::Component::RigidBody>(core, rb);
    
    // Box collider (chassis)
    Physics::Component::BoxCollider collider(glm::vec3(1.0f, 0.5f, 2.0f));
    car.AddComponent<Physics::Component::BoxCollider>(core, collider);
    
    // Vehicle component
    auto vehicle = Physics::Component::Vehicle::Create(
        Physics::Component::VehicleSettings::DefaultCar()
    );
    car.AddComponent<Physics::Component::Vehicle>(core, vehicle);
    
    // Create 4 wheels
    auto wheelFL = CreateWheel(core, car, glm::vec3(0.8f, -0.3f, 1.2f),
                               Physics::Component::WheelSettings::Front(false),
                               Physics::Component::WheelPosition::FrontLeft);
    
    auto wheelFR = CreateWheel(core, car, glm::vec3(-0.8f, -0.3f, 1.2f),
                               Physics::Component::WheelSettings::Front(false),
                               Physics::Component::WheelPosition::FrontRight);
    
    auto wheelRL = CreateWheel(core, car, glm::vec3(0.8f, -0.3f, -1.2f),
                               Physics::Component::WheelSettings::Rear(true),  // Driven
                               Physics::Component::WheelPosition::RearLeft);
    
    auto wheelRR = CreateWheel(core, car, glm::vec3(-0.8f, -0.3f, -1.2f),
                               Physics::Component::WheelSettings::Rear(true),  // Driven
                               Physics::Component::WheelPosition::RearRight);
    
    // Add wheels to vehicle
    vehicle.AddWheel(wheelFL);
    vehicle.AddWheel(wheelFR);
    vehicle.AddWheel(wheelRL);
    vehicle.AddWheel(wheelRR);
    
    Logger::Info("Car created (RWD)");
    return car;
}

// Helper: Create wheel
Engine::Entity CreateWheel(Engine::Core &core, Engine::Entity vehicle,
                          glm::vec3 localPos, Physics::Component::WheelSettings settings,
                          Physics::Component::WheelPosition position) {
    auto wheel = core.CreateEntity();
    wheel.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(localPos));
    
    auto wheelComp = Physics::Component::Wheel::Create(settings, position);
    wheelComp.vehicle = vehicle;
    wheel.AddComponent<Physics::Component::Wheel>(core, wheelComp);
    
    return wheel;
}
```

### Example 2: Control Car (Input Integration)

```cpp
// Update car based on input (integrate with Input plugin)
void UpdateCar(Engine::Core &core, Engine::Entity car) {
    auto &inputManager = core.GetResource<Input::Resource::InputManager>();
    
    // Throttle (W key)
    float throttle = 0.0f;
    if (inputManager.IsKeyPressed(GLFW_KEY_W)) {
        throttle = 1.0f;
    }
    Physics::Resource::VehicleController::SetThrottle(core, car, throttle);
    
    // Brake (S key)
    float brake = 0.0f;
    if (inputManager.IsKeyPressed(GLFW_KEY_S)) {
        brake = 1.0f;
    }
    Physics::Resource::VehicleController::SetBrake(core, car, brake);
    
    // Steering (A/D keys)
    float steer = 0.0f;
    if (inputManager.IsKeyPressed(GLFW_KEY_A)) steer -= 1.0f;
    if (inputManager.IsKeyPressed(GLFW_KEY_D)) steer += 1.0f;
    Physics::Resource::VehicleController::SetSteering(core, car, steer);
    
    // Handbrake (Space)
    bool handBrake = inputManager.IsKeyPressed(GLFW_KEY_SPACE);
    Physics::Resource::VehicleController::SetHandBrake(core, car, handBrake);
    
    // Shift up/down (Q/E)
    static bool qPressed = false, ePressed = false;
    if (inputManager.IsKeyPressed(GLFW_KEY_Q) && !qPressed) {
        Physics::Resource::VehicleController::ShiftDown(core, car);
        qPressed = true;
    } else if (!inputManager.IsKeyPressed(GLFW_KEY_Q)) {
        qPressed = false;
    }
    
    if (inputManager.IsKeyPressed(GLFW_KEY_E) && !ePressed) {
        Physics::Resource::VehicleController::ShiftUp(core, car);
        ePressed = true;
    } else if (!inputManager.IsKeyPressed(GLFW_KEY_E)) {
        ePressed = false;
    }
    
    // Reset (R key)
    if (inputManager.IsKeyPressed(GLFW_KEY_R)) {
        Physics::Resource::VehicleController::ResetVehicle(core, car);
    }
}
```

### Example 3: Display Vehicle Info (HUD)

```cpp
// Display vehicle stats (speed, RPM, gear)
void DisplayVehicleInfo(Engine::Core &core, Engine::Entity car) {
    auto &vehicle = car.GetComponent<Physics::Component::Vehicle>(core);
    
    Logger::Info("===== Vehicle Info =====");
    Logger::Info("Speed: {:.1f} km/h ({:.1f} mph)",
                 vehicle.GetSpeedKmh(), vehicle.GetSpeedMph());
    Logger::Info("Engine RPM: {:.0f}", vehicle.engineRPM);
    Logger::Info("Gear: {}", vehicle.currentGear);
    Logger::Info("Throttle: {:.0f}%", vehicle.throttle * 100.0f);
    Logger::Info("Brake: {:.0f}%", vehicle.brake * 100.0f);
    Logger::Info("Steering: {:.2f}", vehicle.steerInput);
    Logger::Info("Handbrake: {}", vehicle.handBrake ? "ON" : "OFF");
    
    // Wheel info
    for (size_t i = 0; i < vehicle.wheels.size(); ++i) {
        auto &wheel = vehicle.wheels[i].GetComponent<Physics::Component::Wheel>(core);
        Logger::Info("Wheel {}: RPM={:.0f}, Slip=({:.2f}, {:.2f}), Ground={}",
                     i, wheel.GetRPM(), wheel.lateralSlip, wheel.longitudinalSlip,
                     wheel.isGrounded ? "YES" : "NO");
    }
}
```

### Example 4: Create Sports Car (AWD, High Power)

```cpp
// Create high-performance sports car
Engine::Entity CreateSportsCar(Engine::Core &core, glm::vec3 spawnPos) {
    auto car = core.CreateEntity();
    car.AddComponent<Object::Component::Transform>(core,
        Object::Component::Transform::Create(spawnPos));
    
    // Lighter body (1200kg)
    auto rb = Physics::Component::RigidBody::CreateDynamic(1200.0f);
    rb.centerOfMass = glm::vec3(0, -0.2f, 0);  // Very low CoM
    car.AddComponent<Physics::Component::RigidBody>(core, rb);
    
    Physics::Component::BoxCollider collider(glm::vec3(0.9f, 0.4f, 2.2f));
    car.AddComponent<Physics::Component::BoxCollider>(core, collider);
    
    // Sports car settings (high power, AWD, downforce)
    auto vehicle = Physics::Component::Vehicle::Create(
        Physics::Component::VehicleSettings::SportsCar()
    );
    car.AddComponent<Physics::Component::Vehicle>(core, vehicle);
    
    // All wheels driven (AWD)
    auto wheelFL = CreateWheel(core, car, glm::vec3(0.75f, -0.25f, 1.3f),
                               Physics::Component::WheelSettings::Front(true),  // Driven
                               Physics::Component::WheelPosition::FrontLeft);
    
    auto wheelFR = CreateWheel(core, car, glm::vec3(-0.75f, -0.25f, 1.3f),
                               Physics::Component::WheelSettings::Front(true),  // Driven
                               Physics::Component::WheelPosition::FrontRight);
    
    auto wheelRL = CreateWheel(core, car, glm::vec3(0.75f, -0.25f, -1.3f),
                               Physics::Component::WheelSettings::Rear(true),
                               Physics::Component::WheelPosition::RearLeft);
    
    auto wheelRR = CreateWheel(core, car, glm::vec3(-0.75f, -0.25f, -1.3f),
                               Physics::Component::WheelSettings::Rear(true),
                               Physics::Component::WheelPosition::RearRight);
    
    vehicle.AddWheel(wheelFL);
    vehicle.AddWheel(wheelFR);
    vehicle.AddWheel(wheelRL);
    vehicle.AddWheel(wheelRR);
    
    Logger::Info("Sports car created (AWD, 500Nm torque)");
    return car;
}
```

### Example 5: Suspension Test Track

```cpp
// Create bumpy track to test suspension
void CreateBumpyTrack(Engine::Core &core, glm::vec3 startPos) {
    const int bumpCount = 10;
    const float bumpSpacing = 5.0f;
    const float bumpHeight = 0.3f;
    
    for (int i = 0; i < bumpCount; ++i) {
        glm::vec3 bumpPos = startPos + glm::vec3(0, 0, i * bumpSpacing);
        
        auto bump = core.CreateEntity();
        bump.AddComponent<Object::Component::Transform>(core,
            Object::Component::Transform::Create(bumpPos));
        
        auto rb = Physics::Component::RigidBody::CreateStatic();
        bump.AddComponent<Physics::Component::RigidBody>(core, rb);
        
        // Half-cylinder bump
        Physics::Component::CapsuleCollider collider(bumpHeight, 2.0f);
        bump.AddComponent<Physics::Component::CapsuleCollider>(core, collider);
    }
    
    Logger::Info("Bumpy track created ({} bumps)", bumpCount);
}
```

### Example 6: Check Wheel Slip (Particle Effects)

```cpp
// Spawn tire smoke when wheels slip
void CheckWheelSlip(Engine::Core &core, Engine::Entity car) {
    auto &vehicle = car.GetComponent<Physics::Component::Vehicle>(core);
    
    for (auto wheelEntity : vehicle.wheels) {
        auto &wheel = wheelEntity.GetComponent<Physics::Component::Wheel>(core);
        
        if (wheel.IsSlipping() && wheel.isGrounded) {
            auto &transform = wheelEntity.GetComponent<Object::Component::Transform>(core);
            
            // Spawn tire smoke particle effect
            Particles::SpawnSmoke(core, transform.position, glm::vec3(0, 0.1f, 0));
            
            // Play skid sound
            Audio::PlaySound("sounds/tire_skid.wav", transform.position);
            
            Logger::Debug("Wheel slipping: lateral={:.2f}, longitudinal={:.2f}",
                         wheel.lateralSlip, wheel.longitudinalSlip);
        }
    }
}
```

## 🐛 Error Handling

### Cases to Handle

1. **No RigidBody**
   ```cpp
   if (!entity.HasComponent<RigidBody>(core)) {
       Logger::Error("Vehicle: Entity missing RigidBody component");
       return;
   }
   ```

2. **No wheels**
   ```cpp
   if (vehicle.wheels.empty()) {
       Logger::Error("Vehicle: No wheels attached");
       return;
   }
   ```

3. **Invalid gear**
   ```cpp
   if (gear > maxGear || gear < -1) {
       Logger::Error("Vehicle: Invalid gear {}", gear);
       return;
   }
   ```

4. **Wheel without vehicle**
   ```cpp
   if (!wheel.vehicle.IsValid(core)) {
       Logger::Error("Wheel: No parent vehicle");
       return;
   }
   ```

## 💡 Design Considerations

### Vehicle Realism vs Arcade

```cpp
// Realistic: Full physics simulation
VehicleSettings realistic;
realistic.engine.maxTorque = 300.0f;
realistic.differentialType = DifferentialType::LimitedSlip;

// Arcade: Simplified, more responsive
VehicleSettings arcade;
arcade.engine.maxTorque = 800.0f;  // More power
arcade.maxSteerAngle = 50.0f;      // Tighter turns
arcade.downforceCoefficient = 2.0f; // More grip
```

### Performance Notes

- Each wheel adds ~1ms overhead
- Suspension calculation is per-frame
- Recommended: max 20 vehicles with 4 wheels each

## 📊 Success Criteria

- [ ] Vehicle component implemented
- [ ] Wheel component implemented
- [ ] Engine simulation working (torque curve)
- [ ] Suspension working (spring-damper)
- [ ] Steering working (Ackermann)
- [ ] Braking working (bias)
- [ ] Gear shifting working
- [ ] Wheel slip detection working
- [ ] VehicleController resource implemented
- [ ] VehicleSystem implemented
- [ ] Jolt integration working
- [ ] Unit tests pass (100% coverage)
- [ ] Integration examples with Input plugin
- [ ] Performance acceptable (< 5ms for 10 vehicles)
- [ ] Documentation complete
- [ ] Code review approved

## 🔗 References

- Jolt Vehicle: https://jrouwe.github.io/JoltPhysics/class_wheeled_vehicle_controller.html
- Vehicle Physics: https://www.asawicki.info/Mirror/Car%20Physics%20for%20Games/Car%20Physics%20for%20Games.html
- Unity WheelCollider: https://docs.unity3d.com/Manual/class-WheelCollider.html
- Ackermann Steering: https://en.wikipedia.org/wiki/Ackermann_steering_geometry

## 🚀 Follow-up Issues

- #020: Debug Visualization (show suspension, wheel forces)
- #024: Constraint Tuning (suspension tuning tools)
- #023: AI Pathfinding (AI drivers)

---

**Assignee:** TBD  
**Labels:** `enhancement`, `physics`, `v0.7`, `priority:high`, `vehicle-physics`  
**Milestone:** v0.7 - Advanced Physics
