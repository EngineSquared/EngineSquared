#include "RopeGenerator.hpp"
#include "Object.pch.hpp"


namespace Object::Utils {

Component::Mesh GenerateRopeMesh(uint32_t segmentCount, float segmentLength)
{
    Component::Mesh mesh;

    if (segmentCount == 0u)
    {
        // Single point rope
        mesh.EmplaceVertices(0.0f, 0.0f, 0.0f);
        mesh.EmplaceNormals(0.0f, 0.0f, 1.0f);
        mesh.EmplaceTexCoords(0.0f, 0.0f);
        return mesh;
    }

    uint32_t vertexCount = segmentCount + 1u;
    mesh.ReserveVertices(vertexCount);
    mesh.ReserveNormals(vertexCount);
    mesh.ReserveTexCoords(vertexCount);

    for (uint32_t i = 0u; i < vertexCount; ++i)
    {
        mesh.EmplaceVertices(0.0f, -static_cast<float>(i) * segmentLength, 0.0f);
        mesh.EmplaceNormals(0.0f, 0.0f, 1.0f);
        mesh.EmplaceTexCoords(0.0f, static_cast<float>(i) / static_cast<float>(segmentCount));
    }

    // No faces for rope (line rendering or soft body only)

    return mesh;
}

} // namespace Object::Utils