#include "scenes/VehicleScene.hpp"
#include "component/PlayerVehicle.hpp"

#include "Graphic.hpp"
#include "Object.hpp"
#include "Physics.hpp"
#include "builder/VehicleBuilder.hpp"
#include "component/Transform.hpp"
#include "component/VehicleController.hpp"

#include <glm/glm.hpp>
#include <iostream>

/**
 * @brief Create a checkered floor (200x200 meters) with alternating grey tiles
 *
 * Uses a single large physics body to avoid ghost collisions at tile edges,
 * while creating separate visual tiles for the checkered pattern.
 */
void CreateCheckeredFloor(Engine::Core &core)
{
    const float tileSize = 10.0f;
    const int tilesPerSide = 20; // 20 tiles * 10m = 200m
    const float totalSize = tileSize * tilesPerSide;
    const float startOffset = -totalSize / 2.0f;

    std::cout << "Creating " << tilesPerSide << "x" << tilesPerSide << " checkered floor..." << std::endl;

    // Create a single large physics floor to avoid ghost collisions at tile edges
    auto floorPhysics = core.CreateEntity();
    floorPhysics.AddComponent<Object::Component::Transform>(core, glm::vec3(0.0f, 0.0f, 0.0f));
    auto floorCollider = Physics::Component::BoxCollider(glm::vec3(totalSize / 2.0f, 0.1f, totalSize / 2.0f));
    floorPhysics.AddComponent<Physics::Component::BoxCollider>(core, floorCollider);
    floorPhysics.AddComponent<Physics::Component::RigidBody>(core, Physics::Component::RigidBody::CreateStatic());

    // Create visual tiles (no physics) for the checkered pattern
    for (int x = 0; x < tilesPerSide; ++x)
    {
        for (int z = 0; z < tilesPerSide; ++z)
        {
            float posX = startOffset + (x * tileSize) + (tileSize / 2.0f);
            float posZ = startOffset + (z * tileSize) + (tileSize / 2.0f);

            bool isLightTile = (x + z) % 2 == 0;
            // Use a black / grey checker pattern
            glm::vec3 color = isLightTile ? glm::vec3(0.6f, 0.6f, 0.6f) : glm::vec3(0.0f, 0.0f, 0.0f);

            auto tile = Object::Helper::CreatePlane(core, {.width = tileSize, .depth = tileSize,
                                                            .position = glm::vec3(posX, 0.0f, posZ),
                                                            .rotation = glm::vec3(0.0f, 0.0f, 0.0f)});

            Object::Component::Material tileMaterial;
            tileMaterial.diffuse = color;
            tileMaterial.ambient = color * 0.3f;
            tileMaterial.specular = glm::vec3(0.1f);
            tileMaterial.shininess = 16.0f;
            // Use engine default texture (1x1) so material color is used without external textures
            tileMaterial.ambientTexName = Graphic::Utils::DEFAULT_TEXTURE_NAME;
            tile.AddComponent<Object::Component::Material>(core, tileMaterial);

            // No physics on visual tiles - single floor body handles collision
        }
    }
}

/**
 * @brief Create a drivable vehicle using VehicleBuilder
 */
Engine::Entity CreateVehicle(Engine::Core &core)
{
    Object::Component::Mesh chassisMesh = Object::Utils::GenerateCubeMesh(1.0f);
    Object::Component::Mesh wheelMesh = Object::Utils::GenerateWheelMesh(0.4f, 0.3f);

    Physics::Component::WheelSettings frontWheel = Physics::Component::WheelSettings::CreateFrontWheel();
    frontWheel.radius = 0.4f;
    frontWheel.width = 0.3f;
    frontWheel.longitudinalFriction = 2.5f;
    frontWheel.lateralFriction = 2.0f;

    Physics::Component::WheelSettings rearWheel = Physics::Component::WheelSettings::CreateRearWheel();
    rearWheel.radius = 0.4f;
    rearWheel.width = 0.3f;
    rearWheel.longitudinalFriction = 2.5f;
    rearWheel.lateralFriction = 2.0f;

    Physics::Builder::VehicleBuilder<4> builder;
    auto vehicleEntity = builder.SetChassisMesh(chassisMesh, glm::vec3(0.0f, 2.0f, 0.0f))
                             .SetWheelMesh(Physics::Component::WheelIndex::FrontLeft, wheelMesh)
                             .SetWheelMesh(Physics::Component::WheelIndex::FrontRight, wheelMesh)
                             .SetWheelMesh(Physics::Component::WheelIndex::RearLeft, wheelMesh)
                             .SetWheelMesh(Physics::Component::WheelIndex::RearRight, wheelMesh)
                             .SetWheelSettings(Physics::Component::WheelIndex::FrontLeft, frontWheel)
                             .SetWheelSettings(Physics::Component::WheelIndex::FrontRight, frontWheel)
                             .SetWheelSettings(Physics::Component::WheelIndex::RearLeft, rearWheel)
                             .SetWheelSettings(Physics::Component::WheelIndex::RearRight, rearWheel)
                             .SetDrivetrain(Physics::Component::DrivetrainType::RWD)
                             .SetChassisMass(1200.0f)
                             .SetChassisHalfExtents(glm::vec3(1.0f, 0.4f, 2.0f))
                             .Build(core);

    Object::Component::Material chassisMaterial;
    // Light blue car color
    chassisMaterial.diffuse = glm::vec3(0.4f, 0.7f, 0.95f);
    chassisMaterial.ambient = chassisMaterial.diffuse * 0.3f;
    chassisMaterial.specular = glm::vec3(0.3f);
    chassisMaterial.shininess = 32.0f;
    // Use engine default texture to avoid missing texture warnings while keeping a plain material
    chassisMaterial.ambientTexName = Graphic::Utils::DEFAULT_TEXTURE_NAME;
    vehicleEntity.AddComponent<Object::Component::Material>(core, chassisMaterial);

    vehicleEntity.AddComponent<PlayerVehicle>(core);
    return vehicleEntity;
}
