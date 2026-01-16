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
    entity.AddComponent<Component::Transform>(transform);

    auto mesh = Utils::GenerateCubeMesh(size);
    entity.AddComponent<Component::Mesh>(std::move(mesh));

    return entity;
}

Engine::Entity CreateSphere(Engine::Core &core, float radius, const glm::vec3 &position, uint32_t segments,
                            uint32_t rings)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position);
    entity.AddComponent<Component::Transform>(transform);

    auto mesh = Utils::GenerateSphereMesh(radius, segments, rings);
    entity.AddComponent<Component::Mesh>(std::move(mesh));

    return entity;
}

Engine::Entity CreatePlane(Engine::Core &core, float width, float depth, const glm::vec3 &position,
                           uint32_t subdivisionsX, uint32_t subdivisionsZ)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position);
    entity.AddComponent<Component::Transform>(transform);

    auto mesh = Utils::GeneratePlaneMesh(width, depth, subdivisionsX, subdivisionsZ);
    entity.AddComponent<Component::Mesh>(std::move(mesh));

    return entity;
}

Engine::Entity CreateCylinder(Engine::Core &core, float radiusTop, float radiusBottom, float height,
                              const glm::vec3 &position, uint32_t segments)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position);
    entity.AddComponent<Component::Transform>(transform);

    auto mesh = Utils::GenerateCylinderMesh(radiusTop, radiusBottom, height, segments);
    entity.AddComponent<Component::Mesh>(std::move(mesh));

    return entity;
}

Engine::Entity CreateCapsule(Engine::Core &core, float radius, float height, const glm::vec3 &position,
                             uint32_t segments, uint32_t heightSegments)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position);
    entity.AddComponent<Component::Transform>(transform);

    auto mesh = Utils::GenerateCapsuleMesh(radius, height, segments, heightSegments);
    entity.AddComponent<Component::Mesh>(std::move(mesh));

    return entity;
}

} // namespace Object::Helper
