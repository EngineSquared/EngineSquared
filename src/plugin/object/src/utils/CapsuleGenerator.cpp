#include "CapsuleGenerator.hpp"
#include "Object.pch.hpp"


namespace Object::Utils {

Component::Mesh GenerateCapsuleMesh(float radius, float height, uint32_t segments, uint32_t heightSegments)
{
    Component::Mesh mesh;

    if (radius <= 0.0f)
    {
        return mesh;
    }
    height = std::max(0.0f, height);

    segments = std::max(3u, segments);
    heightSegments = std::max(1u, heightSegments);

    uint32_t capRings = std::max(2u, heightSegments * 2u);
    float halfHeight = height * 0.5f;

    float totalHeight = height + 2.0f * radius;
    if (totalHeight <= std::numeric_limits<float>::epsilon())
        totalHeight = 1.0f;

    uint32_t ringCount = capRings + (heightSegments + 1u) + capRings;
    uint32_t vertexEstimate = ringCount * (segments + 1u);
    mesh.ReserveVertices(vertexEstimate);
    mesh.ReserveNormals(vertexEstimate);
    mesh.ReserveTexCoords(vertexEstimate);

    auto addRing = [&](float y, float r, float centerY) {
        for (uint32_t s = 0; s <= segments; ++s)
        {
            float theta = static_cast<float>(s) / static_cast<float>(segments) * 2.0f * glm::pi<float>();
            float cosT = std::cos(theta);
            float sinT = std::sin(theta);
            glm::vec3 vertex(r * cosT, y, r * sinT);
            mesh.EmplaceVertices(vertex);
            glm::vec3 normal = glm::normalize(glm::vec3(r * cosT, y - centerY, r * sinT));
            mesh.EmplaceNormals(normal);
            mesh.EmplaceTexCoords(static_cast<float>(s) / static_cast<float>(segments),
                                  (y + (radius + halfHeight)) / totalHeight);
        }
    };

    // Top hemisphere (exclude equator)
    for (uint32_t ring = 0; ring < capRings; ++ring)
    {
        float t = static_cast<float>(ring) / static_cast<float>(capRings);
        float phi = t * (glm::pi<float>() * 0.5f);
        float r = radius * std::sin(phi);
        float y = halfHeight + radius * std::cos(phi);
        addRing(y, r, halfHeight);
    }

    // Cylinder rings (including top and bottom edges)
    for (uint32_t h = 0; h <= heightSegments; ++h)
    {
        float t = static_cast<float>(h) / static_cast<float>(heightSegments);
        float y = halfHeight - t * height;
        addRing(y, radius, y); // centerY == y results in purely horizontal normal
    }

    // Bottom hemisphere (exclude equator)
    for (uint32_t ring = 0; ring < capRings; ++ring)
    {
        float t = static_cast<float>(ring) / static_cast<float>(capRings);
        float phi = t * (glm::pi<float>() * 0.5f);
        float r = radius * std::sin(phi);
        float y = -halfHeight - radius * std::cos(phi);
        addRing(y, r, -halfHeight);
    }

    // Build indices between rings
    uint32_t ringsTotal = capRings + (heightSegments + 1u) + capRings;
    // Build triangle indices between consecutive rings. Stop at ringsTotal-1 so
    // nextStart (ring+1) is always valid.
    for (uint32_t ring = 0; ring + 1u < ringsTotal; ++ring)
    {
        uint32_t ringStart = ring * (segments + 1u);
        uint32_t nextStart = (ring + 1u) * (segments + 1u);
        for (uint32_t s = 0; s < segments; ++s)
        {
            uint32_t i0 = ringStart + s;
            uint32_t i1 = ringStart + s + 1u;
            uint32_t i2 = nextStart + s;
            uint32_t i3 = nextStart + s + 1u;

            mesh.EmplaceIndices(i0);
            mesh.EmplaceIndices(i2);
            mesh.EmplaceIndices(i1);
            mesh.EmplaceIndices(i1);
            mesh.EmplaceIndices(i2);
            mesh.EmplaceIndices(i3);
        }
    }

    return mesh;
}

} // namespace Object::Utils