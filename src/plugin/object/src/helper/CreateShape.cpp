#include "CreateShape.hpp"
#include "Object.pch.hpp"
#include "utils/ShapeGenerator.hpp"

#include <cmath>

namespace Object::Helper {

Engine::Entity CreateCube(Engine::Core &core, float size, const glm::vec3 &position, const glm::quat &rotation,
                          const glm::vec3 &scale)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position, scale, rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = Utils::GenerateCubeMesh(size);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreateSphere(Engine::Core &core, float radius, const glm::vec3 &position, const glm::quat &rotation,
                            const glm::vec3 &scale, uint32_t segments, uint32_t rings)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position, scale, rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = Utils::GenerateSphereMesh(radius, segments, rings);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreatePlane(Engine::Core &core, float width, float depth, const glm::vec3 &position,
                           const glm::quat &rotation, const glm::vec3 &scale, uint32_t subdivisionsX,
                           uint32_t subdivisionsZ)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position, scale, rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = Utils::GeneratePlaneMesh(width, depth, subdivisionsX, subdivisionsZ);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreateCylinder(Engine::Core &core, float radiusTop, float radiusBottom, float height,
                              const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale,
                              uint32_t segments)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position, scale, rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = Utils::GenerateCylinderMesh(radiusTop, radiusBottom, height, segments);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreateCapsule(Engine::Core &core, float radius, float height, const glm::vec3 &position,
                             const glm::quat &rotation, const glm::vec3 &scale, uint32_t segments,
                             uint32_t heightSegments)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position, scale, rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = Utils::GenerateCapsuleMesh(radius, height, segments, heightSegments);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreateCloth(Engine::Core &core, uint32_t width, uint32_t height, float spacing,
                           const glm::vec3 &position, const glm::quat &rotation, const glm::vec3 &scale)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position, scale, rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = Utils::GenerateClothMesh(width, height, spacing);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreateRope(Engine::Core &core, uint32_t segmentCount, float segmentLength, const glm::vec3 &position,
                          const glm::quat &rotation, const glm::vec3 &scale)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position, scale, rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = Utils::GenerateRopeMesh(segmentCount, segmentLength);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreateJellyCube(Engine::Core &core, float size, uint32_t gridSize, const glm::vec3 &position,
                               const glm::quat &rotation, const glm::vec3 &scale)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position, scale, rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    float spacing = (gridSize > 1) ? size / static_cast<float>(gridSize - 1) : size;
    auto mesh = Utils::GenerateJellyCubeMesh(gridSize, spacing);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

} // namespace Object::Helper
