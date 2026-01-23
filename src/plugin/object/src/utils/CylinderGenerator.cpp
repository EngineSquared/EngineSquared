#include "CylinderGenerator.hpp"
#include "Object.pch.hpp"

namespace Object::Utils {

Component::Mesh GenerateCylinderMesh(float radiusTop, float radiusBottom, float height, uint32_t segments,
                                     uint32_t heightSegments)
{
    Component::Mesh mesh;

    segments = std::max(3u, segments);
    heightSegments = std::max(1u, heightSegments);

    float halfHeight = height * 0.5f;
    float slopeAngle = std::atan2(radiusBottom - radiusTop, height);
    float cosSlope = std::cos(slopeAngle);
    float sinSlope = std::sin(slopeAngle);

    uint32_t sideVertexCount = (heightSegments + 1u) * (segments + 1u);
    uint32_t capVertexCount = 2u * (segments + 2u); // Top and bottom caps (center + ring)
    mesh.ReserveVertices(sideVertexCount + capVertexCount);
    mesh.ReserveNormals(sideVertexCount + capVertexCount);
    mesh.ReserveTexCoords(sideVertexCount + capVertexCount);
    mesh.ReserveIndices(heightSegments * segments * 6u + segments * 6u); // Sides + caps

    for (uint32_t h = 0u; h <= heightSegments; ++h)
    {
        float y = -halfHeight + (static_cast<float>(h) / static_cast<float>(heightSegments)) * height;
        float t = static_cast<float>(h) / static_cast<float>(heightSegments);
        float radius = std::lerp(radiusBottom, radiusTop, t);

        for (uint32_t seg = 0; seg <= segments; ++seg)
        {
            float theta = static_cast<float>(seg) / static_cast<float>(segments) * 2.0f * glm::pi<float>();
            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            glm::vec3 vertex(radius * cosTheta, y, radius * sinTheta);
            mesh.EmplaceVertices(vertex);

            glm::vec3 normal = glm::normalize(glm::vec3(cosTheta * cosSlope, sinSlope, sinTheta * cosSlope));
            mesh.EmplaceNormals(normal);

            mesh.EmplaceTexCoords(static_cast<float>(seg) / static_cast<float>(segments), t);
        }
    }

    for (uint32_t h = 0u; h < heightSegments; ++h)
    {
        for (uint32_t seg = 0u; seg < segments; ++seg)
        {
            uint32_t current = h * (segments + 1u) + seg;
            uint32_t next = current + segments + 1u;

            mesh.EmplaceIndices(current);
            mesh.EmplaceIndices(next);
            mesh.EmplaceIndices(current + 1u);

            mesh.EmplaceIndices(current + 1u);
            mesh.EmplaceIndices(next);
            mesh.EmplaceIndices(next + 1u);
        }
    }

    // Generate top cap (if radius > 0)
    if (radiusTop > 0.0f)
    {
        auto centerTop = static_cast<uint32_t>(mesh.GetVertices().size());
        mesh.EmplaceVertices(0.0f, halfHeight, 0.0f);
        mesh.EmplaceNormals(0.0f, 1.0f, 0.0f);
        mesh.EmplaceTexCoords(0.5f, 0.5f);

        for (uint32_t seg = 0; seg <= segments; ++seg)
        {
            float theta = static_cast<float>(seg) / static_cast<float>(segments) * 2.0f * glm::pi<float>();
            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            mesh.EmplaceVertices(radiusTop * cosTheta, halfHeight, radiusTop * sinTheta);
            mesh.EmplaceNormals(0.0f, 1.0f, 0.0f);
            mesh.EmplaceTexCoords(0.5f + 0.5f * cosTheta, 0.5f + 0.5f * sinTheta);
        }

        for (uint32_t seg = 0u; seg < segments; ++seg)
        {
            mesh.EmplaceIndices(centerTop);
            mesh.EmplaceIndices(centerTop + seg + 2u);
            mesh.EmplaceIndices(centerTop + seg + 1u);
        }
    }

    // Generate bottom cap (if radius > 0)
    if (radiusBottom <= 0.0f)
        return mesh;

    auto centerBottom = static_cast<uint32_t>(mesh.GetVertices().size());
    mesh.EmplaceVertices(0.0f, -halfHeight, 0.0f);
    mesh.EmplaceNormals(0.0f, -1.0f, 0.0f);
    mesh.EmplaceTexCoords(0.5f, 0.5f);

    for (uint32_t seg = 0u; seg <= segments; ++seg)
    {
        float theta = static_cast<float>(seg) / static_cast<float>(segments) * 2.0f * glm::pi<float>();
        float cosTheta = std::cos(theta);
        float sinTheta = std::sin(theta);

        mesh.EmplaceVertices(radiusBottom * cosTheta, -halfHeight, radiusBottom * sinTheta);
        mesh.EmplaceNormals(0.0f, -1.0f, 0.0f);
        mesh.EmplaceTexCoords(0.5f + 0.5f * cosTheta, 0.5f + 0.5f * sinTheta);
    }

    for (uint32_t seg = 0u; seg < segments; ++seg)
    {
        mesh.EmplaceIndices(centerBottom);
        mesh.EmplaceIndices(centerBottom + seg + 1u);
        mesh.EmplaceIndices(centerBottom + seg + 2u);
    }

    return mesh;
}

} // namespace Object::Utils
