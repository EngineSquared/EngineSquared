#include "SphereGenerator.hpp"
#include "Object.pch.hpp"

namespace Object::Utils {

Component::Mesh GenerateSphereMesh(float radius, uint32_t segments, uint32_t rings)
{
    Component::Mesh mesh;

    segments = std::max(3u, segments);
    rings = std::max(2u, rings);

    uint32_t vertexCount = (rings + 1u) * (segments + 1u);
    mesh.ReserveVertices(vertexCount);
    mesh.ReserveNormals(vertexCount);
    mesh.ReserveTexCoords(vertexCount);
    mesh.ReserveIndices(rings * segments * 6u);

    for (uint32_t ring = 0u; ring <= rings; ++ring)
    {
        float phi = static_cast<float>(ring) / static_cast<float>(rings) * glm::pi<float>();
        float sinPhi = std::sin(phi);
        float cosPhi = std::cos(phi);

        for (uint32_t seg = 0u; seg <= segments; ++seg)
        {
            float theta = static_cast<float>(seg) / static_cast<float>(segments) * 2.0f * glm::pi<float>();
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            glm::vec3 vertex(radius * sinPhi * cosTheta, radius * cosPhi, radius * sinPhi * sinTheta);
            mesh.EmplaceVertices(vertex);

            glm::vec3 normal = glm::normalize(vertex);
            mesh.EmplaceNormals(normal);

            mesh.EmplaceTexCoords(static_cast<float>(seg) / static_cast<float>(segments),
                                  static_cast<float>(ring) / static_cast<float>(rings));
        }
    }

    for (uint32_t ring = 0u; ring < rings; ++ring)
    {
        for (uint32_t seg = 0u; seg < segments; ++seg)
        {
            uint32_t current = ring * (segments + 1u) + seg;
            uint32_t next = current + segments + 1u;

            mesh.EmplaceIndices(current);
            mesh.EmplaceIndices(next);
            mesh.EmplaceIndices(current + 1u);

            mesh.EmplaceIndices(current + 1u);
            mesh.EmplaceIndices(next);
            mesh.EmplaceIndices(next + 1u);
        }
    }

    return mesh;
}

} // namespace Object::Utils
