#include "CreateShape.hpp"
#include "Object.pch.hpp"
#include "utils/ShapeGenerator.hpp"

#include <cmath>

namespace Object::Helper {

Engine::Entity CreateCube(Engine::Core &core, CreateCubeInfo info)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(info.position, info.scale, info.rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = Utils::GenerateCubeMesh(info.size);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreateSphere(Engine::Core &core, CreateSphereInfo info)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(info.position, info.scale, info.rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = Utils::GenerateSphereMesh(info.radius, info.segments, info.rings);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreatePlane(Engine::Core &core, CreatePlaneInfo info)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(info.position, info.scale, info.rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = Utils::GeneratePlaneMesh(info.width, info.depth, info.subdivisionsX, info.subdivisionsZ);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreateCylinder(Engine::Core &core, CreateCylinderInfo info)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(info.position, info.scale, info.rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh =
        Utils::GenerateCylinderMesh(info.radiusTop, info.radiusBottom, info.height, info.segments, info.heightSegments);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreateCapsule(Engine::Core &core, CreateCapsuleInfo info)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(info.position, info.scale, info.rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = Utils::GenerateCapsuleMesh(info.radius, info.height, info.segments, info.heightSegments);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreateCloth(Engine::Core &core, CreateClothInfo info)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(info.position, info.scale, info.rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = Utils::GenerateClothMesh(info.width, info.height, info.spacing);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreateRope(Engine::Core &core, CreateRopeInfo info)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(info.position, info.scale, info.rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = Utils::GenerateRopeMesh(info.segmentCount, info.segmentLength);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreateJellyCube(Engine::Core &core, CreateJellyCubeInfo info)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(info.position, info.scale, info.rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    float spacing = (info.gridSize > 1) ? info.size / static_cast<float>(info.gridSize - 1) : info.size;
    auto mesh = Utils::GenerateJellyCubeMesh(info.gridSize, spacing);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

} // namespace Object::Helper
