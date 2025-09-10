#include "JoltPhysics.pch.hpp"

#include "PhysicsUpdateSoftBody.hpp"

#include "Logger.hpp"
#include "component/Mesh.hpp"
#include "component/RigidBody3D.hpp"
#include "component/SoftBody3D.hpp"
#include "component/Transform.hpp"
#include "resource/PhysicsManager.hpp"
#include "scheduler/FixedTimeUpdate.hpp"

#include <fmt/format.h>

void ES::Plugin::Physics::System::LinkSoftBodiesToPhysicsSystem(entt::registry &registry, entt::entity entity)
{
    auto &softBody = registry.get<ES::Plugin::Physics::Component::SoftBody3D>(entity);
    if (softBody.body != nullptr)
    {
        return;
    }

    if (!registry.all_of<ES::Plugin::Object::Component::Mesh>(entity))
    {
        ES::Utils::Log::Error(
            fmt::format("Failed to create soft body for entity {}: no mesh component", static_cast<uint32_t>(entity)));
        return;
    }

    if (!registry.all_of<ES::Plugin::Object::Component::Transform>(entity))
    {
        registry.emplace<ES::Plugin::Object::Component::Transform>(entity);
        ES::Utils::Log::Warn(fmt::format("Entity {} does not have a transform component, creating one for soft body",
                                         static_cast<uint32_t>(entity)));
    }

    auto &initialTransform = registry.get<ES::Plugin::Object::Component::Transform>(entity);
    auto &mesh = registry.get<ES::Plugin::Object::Component::Mesh>(entity);

    softBody.settings = std::make_shared<JPH::SoftBodySharedSettings>();

    softBody.settings->SetEmbedded();

    // Copy the vertices from the mesh to the soft body settings
    softBody.settings->mVertices.resize(mesh.vertices.size());

    for (size_t i = 0; i < mesh.vertices.size(); i++)
    {
        JPH::SoftBodySharedSettings::Vertex v(JPH::Float3(mesh.vertices[i].x, mesh.vertices[i].y, mesh.vertices[i].z));
        v.mVelocity = JPH::Float3(softBody.vertexSettings.initialVelocity.x, softBody.vertexSettings.initialVelocity.y,
                                  softBody.vertexSettings.initialVelocity.z);
        v.mInvMass = softBody.vertexSettings.invMass;
        softBody.settings->mVertices[i] = v;
    }

    // Create faces from the mesh triangles
    for (size_t i = 0; i < mesh.indices.size(); i += 3)
    {
        JPH::SoftBodySharedSettings::Face f(mesh.indices[i], mesh.indices[i + 1], mesh.indices[i + 2]);
        softBody.settings->AddFace(f);
    }

    // Create constraints
    softBody.settings->CreateConstraints(&softBody.vertexAttributes, 1,
                                         JPH::SoftBodySharedSettings::EBendType::Distance);

    if (softBody.calculateVolumeConstraintVolumes)
    {
        softBody.settings->CalculateVolumeConstraintVolumes();
    }
    if (softBody.calculateSkinnedConstraintNormals)
    {
        softBody.settings->CalculateSkinnedConstraintNormals();
    }

    softBody.settings->Optimize();

    // Add the soft body to the physics system
    auto &physicsManager = registry.ctx().get<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &physicsSystem = physicsManager.GetPhysicsSystem();

    JPH::SoftBodyCreationSettings creationSettings(
        softBody.settings.get(),
        JPH::RVec3(initialTransform.position.x, initialTransform.position.y, initialTransform.position.z),
        JPH::Quat(initialTransform.rotation.x, initialTransform.rotation.y, initialTransform.rotation.z,
                  initialTransform.rotation.w),
        softBody.layer);

    softBody.creationSettings.TransferTo(creationSettings);

    softBody.body = physicsSystem.GetBodyInterface().CreateSoftBody(creationSettings);

    if (softBody.body == nullptr)
    {
        ES::Utils::Log::Error(
            fmt::format("Failed to create soft body for entity {}: returned nullptr", static_cast<uint32_t>(entity)));
        return;
    }

    softBody.body->SetUserData(entt::to_integral(entity));

    physicsSystem.GetBodyInterface().AddBody(softBody.body->GetID(), JPH::EActivation::Activate);
}

void ES::Plugin::Physics::System::UnlinkSoftBodiesToPhysicsSystem(entt::registry &registry, entt::entity entity)
{
    auto &softBody = registry.get<ES::Plugin::Physics::Component::SoftBody3D>(entity);
    if (softBody.body == nullptr)
    {
        return;
    }

    auto &physicsManager = registry.ctx().get<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &physicsSystem = physicsManager.GetPhysicsSystem();

    physicsSystem.GetBodyInterface().RemoveBody(softBody.body->GetID());
    softBody.body = nullptr;
}

void ES::Plugin::Physics::System::OnConstructLinkSoftBodiesToPhysicsSystem(ES::Engine::Core &core)
{
    core.GetRegistry()
        .on_construct<ES::Plugin::Physics::Component::SoftBody3D>()
        .connect<&ES::Plugin::Physics::System::LinkSoftBodiesToPhysicsSystem>();
    core.GetRegistry()
        .on_destroy<ES::Plugin::Physics::Component::SoftBody3D>()
        .connect<&ES::Plugin::Physics::System::UnlinkSoftBodiesToPhysicsSystem>();
}

static void UpdateSoftBodyEntity(ES::Engine::Core &core, ES::Plugin::Physics::Component::SoftBody3D &softBody,
                                 ES::Plugin::Object::Component::Transform &transform,
                                 ES::Plugin::Object::Component::Mesh &mesh)
{
    auto &physicsManager = core.GetResource<ES::Plugin::Physics::Resource::PhysicsManager>();
    auto &physicsSystem = physicsManager.GetPhysicsSystem();
    auto &bodyInterface = physicsSystem.GetBodyInterface();

    // Get transformed shape
    auto transformedShape = bodyInterface.GetTransformedShape(softBody.body->GetID());

    // Set the transform from the transformed shape
    auto updTransform = transformedShape.GetWorldTransform();

    transform.position.x = updTransform.GetTranslation().GetX();
    transform.position.y = updTransform.GetTranslation().GetY();
    transform.position.z = updTransform.GetTranslation().GetZ();

    transform.rotation.w = updTransform.GetRotation().GetQuaternion().GetW();
    transform.rotation.x = updTransform.GetRotation().GetQuaternion().GetX();
    transform.rotation.y = updTransform.GetRotation().GetQuaternion().GetY();
    transform.rotation.z = updTransform.GetRotation().GetQuaternion().GetZ();

    // Set the mesh vertices from the transformed shape
    // TODO: may be best to update existing vertices instead
    mesh.vertices.clear();
    mesh.normals.clear();
    mesh.indices.clear();

    JPH::TransformedShape::GetTrianglesContext context;
    transformedShape.GetTrianglesStart(context, JPH::AABox::sBiggest(),
                                       JPH::RVec3(transform.position.x, transform.position.y, transform.position.z));

    std::array<JPH::Float3, JPH::Shape::cGetTrianglesMinTrianglesRequested * 3> vertices;

    int triangleCount = 0;

    while ((triangleCount = transformedShape.GetTrianglesNext(context, JPH::Shape::cGetTrianglesMinTrianglesRequested,
                                                              vertices.data())) > 0)
    {
        for (int i = 0; i < triangleCount * 3; i += 3)
        {
            mesh.vertices.emplace_back(vertices[i].x, vertices[i].y, vertices[i].z);
            mesh.vertices.emplace_back(vertices[i + 1].x, vertices[i + 1].y, vertices[i + 1].z);
            mesh.vertices.emplace_back(vertices[i + 2].x, vertices[i + 2].y, vertices[i + 2].z);
        }

        for (int i = 0; i < triangleCount * 3; i += 3)
        {
            JPH::Vec3 v0(vertices[i].x, vertices[i].y, vertices[i].z);
            JPH::Vec3 v1(vertices[i + 1].x, vertices[i + 1].y, vertices[i + 1].z);
            JPH::Vec3 v2(vertices[i + 2].x, vertices[i + 2].y, vertices[i + 2].z);

            JPH::Vec3 normal = (v1 - v0).Cross(v2 - v0).Normalized();

            mesh.normals.emplace_back(normal.GetX(), normal.GetY(), normal.GetZ());
            mesh.normals.emplace_back(normal.GetX(), normal.GetY(), normal.GetZ());
            mesh.normals.emplace_back(normal.GetX(), normal.GetY(), normal.GetZ());
        }
    }

    mesh.indices.reserve(mesh.vertices.size());

    for (size_t i = 0; i < mesh.vertices.size(); i += 3)
    {
        mesh.indices.push_back(i);
        mesh.indices.push_back(i + 1);
        mesh.indices.push_back(i + 2);
    }
}

// Sync transform and deformed mesh
void ES::Plugin::Physics::System::SyncSoftBodiesData(ES::Engine::Core &core)
{
    core.GetRegistry()
        .view<ES::Plugin::Physics::Component::SoftBody3D, ES::Plugin::Object::Component::Transform,
              ES::Plugin::Object::Component::Mesh>()
        .each([&](auto &softBody, auto &transform, auto &mesh) {
            UpdateSoftBodyEntity(core, softBody, transform, mesh);
        });
}
