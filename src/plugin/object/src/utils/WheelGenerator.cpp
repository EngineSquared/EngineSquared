#include "WheelGenerator.hpp"
#include "CylinderGenerator.hpp"
#include "Object.pch.hpp"


namespace Object::Utils {

Component::Mesh GenerateWheelMesh(float radius, float width, uint32_t segments)
{
    Component::Mesh cylinderMesh = GenerateCylinderMesh(radius, radius, width, segments, 1u);

    Component::Mesh mesh;
    mesh.ReserveVertices(cylinderMesh.GetVertices().size());
    mesh.ReserveNormals(cylinderMesh.GetNormals().size());
    mesh.SetTexCoords(cylinderMesh.GetTexCoords());
    mesh.SetIndices(cylinderMesh.GetIndices());

    for (const auto &vertex : cylinderMesh.GetVertices())
    {
        mesh.EmplaceVertices(vertex.y, -vertex.x, vertex.z);
    }

    for (const auto &normal : cylinderMesh.GetNormals())
    {
        mesh.EmplaceNormals(normal.y, -normal.x, normal.z);
    }

    return mesh;
}

} // namespace Object::Utils