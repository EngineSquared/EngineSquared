#pragma once

#include "Core.hpp"
#include "Mesh.hpp"
#include "WheeledVehicle3D.hpp"
#include "WheeledVehicleBuilderError.hpp"
#include <array>
#include <functional>
#include <glm/glm.hpp>
#include <optional>
#include <vector>

namespace ES::Plugin::Physics::Utils {
/// @brief A builder class for creating a wheeled vehicle.
/// @note This class is used to create a wheeled vehicle and add it to the physics world.
template <size_t WheelCount = 4> class WheeledVehicleBuilder {
    static_assert(WheelCount > 0, "WheelCount must be greater than 0");

  public:
    /// @brief Create a wheeled vehicle builder.
    /// @param core The core of the engine.
    WheeledVehicleBuilder(ES::Engine::Core &core) : core(core), wheelSettings()
    {
        for (size_t i = 0; i < WheelCount; ++i)
        {
            wheelSettings[i] = std::make_unique<JPH::WheelSettingsWV>();
            wheelSettings[i]->SetEmbedded();
        }
        collisionTester =
            std::make_shared<JPH::VehicleCollisionTesterCastCylinder>(ES::Plugin::Physics::Utils::Layers::MOVING, 0.05);
        collisionTester->SetEmbedded();
    }

    /// @brief Set the right vector of the vehicle.
    /// @param right The right vector to set.
    /// @return A reference to the vehicle builder.
    inline WheeledVehicleBuilder &SetRightVector(const glm::vec3 &right)
    {
        rightVector = right;
        return *this;
    }

    /// @brief Set the up vector of the vehicle.
    /// @param up The up vector to set.
    /// @return A reference to the vehicle builder.
    inline WheeledVehicleBuilder &SetUpVector(const glm::vec3 &up)
    {
        upVector = up;
        return *this;
    }

    /// @brief Set the initial position of the vehicle.
    /// @param position The initial position to set.
    /// @return A reference to the vehicle builder.
    inline WheeledVehicleBuilder &SetInitialPosition(const glm::vec3 &position)
    {
        initialPosition = position;
        return *this;
    }

    /// @brief Set the mass of the vehicle.
    /// @param mass The mass to set.
    /// @return A reference to the vehicle builder.
    inline WheeledVehicleBuilder &SetMass(float mass)
    {
        vehicleMass = mass;
        return *this;
    }

    /// @brief Set the body mesh of the vehicle.
    /// @param mesh The mesh to set.
    /// @return A reference to the vehicle builder.
    inline WheeledVehicleBuilder &SetBodyMesh(const ES::Plugin::Object::Component::Mesh &mesh)
    {
        bodyMesh.emplace(mesh);
        return *this;
    }

    /// @brief Set the wheel mesh of the vehicle.
    /// @param mesh The mesh to set.
    /// @return A reference to the vehicle builder.
    inline WheeledVehicleBuilder &SetWheelMesh(const ES::Plugin::Object::Component::Mesh &mesh)
    {
        wheelMesh.emplace(mesh);
        return *this;
    }

    /// @brief Set the offset center of mass of the vehicle.
    /// @param offset The offset to set.
    /// @return A reference to the vehicle builder.
    inline WheeledVehicleBuilder &SetOffsetCenterOfMass(const glm::vec3 &offset)
    {
        offsetCenterOfMassShape = offset;
        return *this;
    }

    /// @brief Edit a wheel of the vehicle.
    /// @param index The index of the wheel to edit.
    /// @param editFn A callable that takes a reference to the wheel settings and modifies it.
    /// @return A reference to the wheel editor.
    /// @note The index must be less than the number of wheels.
    /// @note Due to the amount of variables that can be set, it is easier to use a lambda function
    /// to set the variables.
    inline WheeledVehicleBuilder &EditWheel(size_t index, const std::function<void(JPH::WheelSettingsWV &)> &editFn)
    {
        if (index >= WheelCount)
            throw WheeledVehicleBuilderError("Index out of range");

        editFn(*wheelSettings[index]);
        return *this;
    }

    /// @brief Create a differential to the vehicle.
    /// @return A reference to the vehicle builder.
    /// @note The differential will be added to the vehicle when it is created.
    inline WheeledVehicleBuilder &CreateDifferential()
    {
        differentialSettings.emplace_back();
        return *this;
    }

    /// @brief Edit a differential of the vehicle.
    /// @param index The index of the differential to edit.
    /// @param editFn A callable that takes a reference to the differential settings and modifies it.
    /// @return A reference to the vehicle builder.
    /// @note The index must be less than the number of differentials.
    /// @note Due to the amount of variables that can be set, it is easier to use a lambda function
    /// to set the variables.
    inline WheeledVehicleBuilder &
    EditDifferential(size_t index, const std::function<void(JPH::VehicleDifferentialSettings &)> &editFn)
    {
        if (index >= differentialSettings.size())
            throw WheeledVehicleBuilderError("Index out of range");

        editFn(differentialSettings[index]);
        return *this;
    }

    /// @brief Create an anti-roll bar to the vehicle.
    /// @return A reference to the vehicle builder.
    /// @note The anti-roll bar will be added to the vehicle when it is created.
    inline WheeledVehicleBuilder &CreateAntiRollBar()
    {
        antiRollBars.emplace_back();
        return *this;
    }

    /// @brief Edit an anti-roll bar of the vehicle.
    /// @param index The index of the anti-roll bar to edit.
    /// @param editFn A callable that takes a reference to the anti-roll bar settings and modifies it.
    /// @return A reference to the vehicle builder.
    /// @note The index must be less than the number of anti-roll bars.
    /// @note Due to the amount of variables that can be set, it is easier to use a lambda function
    /// to set the variables.
    inline WheeledVehicleBuilder &EditAntiRollBar(size_t index,
                                                  const std::function<void(JPH::VehicleAntiRollBar &)> &editFn)
    {
        if (index >= antiRollBars.size())
            throw WheeledVehicleBuilderError("Index out of range");

        editFn(antiRollBars[index]);
        return *this;
    }

    /// @brief Set the function to modify the constraint settings before creating the vehicle.
    /// @param fn A callable that takes a reference to the constraint settings and modifies it.
    /// @return A reference to the vehicle builder.
    /// @note This function is called before the vehicle is created.
    inline WheeledVehicleBuilder &
    SetConstraintSettingsFn(const std::function<void(JPH::VehicleConstraintSettings &)> &fn)
    {
        constraintSettingsFn = fn;
        return *this;
    }

    /// @brief Set a callback to be called when a vehicle wheel is created.
    /// @param fn A callable that takes a reference to the wheel entity and modifies it.
    /// @return A reference to the vehicle builder.
    /// @note This function is called when a wheel is created.
    inline WheeledVehicleBuilder &
    SetWheelCallbackFn(const std::function<void(ES::Engine::Core &, ES::Engine::Entity &)> &fn)
    {
        wheelCallbackFn = fn;
        return *this;
    }

    /// @brief Set a wheel offset from the vehicle initial position.
    /// @param index The index of the wheel to set the offset for.
    /// @param offset The offset to set.
    /// @return A reference to the vehicle builder.
    /// @note The index must be less than the number of wheels.
    inline WheeledVehicleBuilder &SetWheelOffset(size_t index, const glm::vec3 &offset)
    {
        if (index >= WheelCount)
            throw WheeledVehicleBuilderError("Index out of range");

        wheelOffsets[index] = offset;
        return *this;
    }

    /// @brief Set a callback to be called when a vehicle is created.
    /// @param fn A callable that takes a reference to the vehicle entity and modifies it.
    /// @return A reference to the vehicle builder.
    /// @note This function is called when the vehicle is created.
    inline WheeledVehicleBuilder &
    SetVehicleCallbackFn(const std::function<void(ES::Engine::Core &, ES::Engine::Entity &)> &fn)
    {
        vehicleCallbackFn = fn;
        return *this;
    }

    /// @brief Set the function to modify the vehicle controller settings before creating the vehicle.
    /// @param fn A callable that takes a reference to the vehicle controller settings and modifies it.
    /// @return A reference to the vehicle builder.
    /// @note This function is called before the vehicle is created.
    inline WheeledVehicleBuilder &
    SetVehicleControllerSettingsFn(const std::function<void(JPH::WheeledVehicleControllerSettings &)> &fn)
    {
        vehicleControllerSettingsFn = fn;
        return *this;
    }

    /// @brief Set the collision tester of the vehicle.
    /// @param tester The collision tester to set.
    /// @return A reference to the vehicle builder.
    /// @note The collision tester should inherit from Jolt's JPH::VehicleCollisionTester.
    /// By default, a cylinder collision tester is used.
    inline WheeledVehicleBuilder &SetCollisionTester(const std::shared_ptr<JPH::VehicleCollisionTester> &tester)
    {
        collisionTester = tester;
        collisionTester->SetEmbedded();
        return *this;
    }

    /// @brief Create the vehicle and add it to the physics world.
    /// @return The entity of the vehicle.
    ES::Engine::Entity Build();

  private:
    /// Reference to the core of the engine.
    ES::Engine::Core &core;
    /// Default right vector of the vehicle.
    glm::vec3 rightVector = glm::vec3(1.0f, 0.0f, 0.0f);
    /// Default up vector of the vehicle.
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    /// Initial vehicle position.
    glm::vec3 initialPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    /// Vehicle mass.
    float vehicleMass = 1500.0f;
    /// Mesh used for the body of the vehicle.
    std::optional<ES::Plugin::Object::Component::Mesh> bodyMesh = std::nullopt;
    /// Mesh used for the wheels of the vehicle.
    std::optional<ES::Plugin::Object::Component::Mesh> wheelMesh = std::nullopt;
    /// Offset center of mass of the vehicle.
    glm::vec3 offsetCenterOfMassShape = glm::vec3(0.0f, 0.0f, 0.0f);
    /// Wheels settings of the vehicle.
    std::array<std::unique_ptr<JPH::WheelSettingsWV>, WheelCount> wheelSettings = {nullptr};
    /// Wheels offset from the vehicle initial position.
    std::array<glm::vec3, WheelCount> wheelOffsets = {glm::vec3(0.0f, 0.0f, 0.0f)};
    /// Wheel callback function to be called when a wheel is created.
    ///  @note This function is called after a wheel is fully initialized.
    std::function<void(ES::Engine::Core &, ES::Engine::Entity &)> wheelCallbackFn = [](ES::Engine::Core &,
                                                                                       ES::Engine::Entity &) {};
    /// Vehicle callback function to be called when a vehicle is created.
    /// @note This function is called after a vehicle is fully initialized.
    std::function<void(ES::Engine::Core &, ES::Engine::Entity &)> vehicleCallbackFn = [](ES::Engine::Core &,
                                                                                         ES::Engine::Entity &) {};
    /// Vehicle controller settings modification function to be called before creating the vehicle.
    /// @note This function is called before the vehicle is created.
    std::function<void(JPH::WheeledVehicleControllerSettings &)> vehicleControllerSettingsFn =
        [](JPH::WheeledVehicleControllerSettings &) {};
    /// Differentials settings of the vehicle.
    std::vector<JPH::VehicleDifferentialSettings> differentialSettings;
    /// Anti-roll bar settings of the vehicle.
    std::vector<JPH::VehicleAntiRollBar> antiRollBars;
    /// Function to modify the constraint settings before creating the vehicle.
    std::function<void(JPH::VehicleConstraintSettings &)> constraintSettingsFn = [](JPH::VehicleConstraintSettings &) {
    };
    /// Vehicle collision tester.
    std::shared_ptr<JPH::VehicleCollisionTester> collisionTester = nullptr;
};
} // namespace ES::Plugin::Physics::Utils

#include "WheeledVehicleBuilder.inl"
