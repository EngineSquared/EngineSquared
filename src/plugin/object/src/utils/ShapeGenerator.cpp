#include "ShapeGenerator.hpp"
#include "Object.pch.hpp"

#include <cmath>
#include <limits>

namespace Object::Utils {

Component::Mesh GenerateCubeMesh(float size)
{
    Component::Mesh mesh;

    float halfSize = size * 0.5f;

    mesh.SetVertices({
        // Front face (+Z)
        {-halfSize, -halfSize, halfSize },
        {halfSize,  -halfSize, halfSize },
        {halfSize,  halfSize,  halfSize },
        {-halfSize, halfSize,  halfSize },

        // Back face (-Z)
        {halfSize,  -halfSize, -halfSize},
        {-halfSize, -halfSize, -halfSize},
        {-halfSize, halfSize,  -halfSize},
        {halfSize,  halfSize,  -halfSize},

        // Top face (+Y)
        {-halfSize, halfSize,  halfSize },
        {halfSize,  halfSize,  halfSize },
        {halfSize,  halfSize,  -halfSize},
        {-halfSize, halfSize,  -halfSize},

        // Bottom face (-Y)
        {-halfSize, -halfSize, -halfSize},
        {halfSize,  -halfSize, -halfSize},
        {halfSize,  -halfSize, halfSize },
        {-halfSize, -halfSize, halfSize },

        // Right face (+X)
        {halfSize,  -halfSize, halfSize },
        {halfSize,  -halfSize, -halfSize},
        {halfSize,  halfSize,  -halfSize},
        {halfSize,  halfSize,  halfSize },

        // Left face (-X)
        {-halfSize, -halfSize, -halfSize},
        {-halfSize, -halfSize, halfSize },
        {-halfSize, halfSize,  halfSize },
        {-halfSize, halfSize,  -halfSize}
    });

    mesh.SetNormals({
        // Front face
        {0,  0,  1 },
        {0,  0,  1 },
        {0,  0,  1 },
        {0,  0,  1 },
        // Back face
        {0,  0,  -1},
        {0,  0,  -1},
        {0,  0,  -1},
        {0,  0,  -1},
        // Top face
        {0,  1,  0 },
        {0,  1,  0 },
        {0,  1,  0 },
        {0,  1,  0 },
        // Bottom face
        {0,  -1, 0 },
        {0,  -1, 0 },
        {0,  -1, 0 },
        {0,  -1, 0 },
        // Right face
        {1,  0,  0 },
        {1,  0,  0 },
        {1,  0,  0 },
        {1,  0,  0 },
        // Left face
        {-1, 0,  0 },
        {-1, 0,  0 },
        {-1, 0,  0 },
        {-1, 0,  0 }
    });

    mesh.SetTexCoords({
        // Front face
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
        // Back face
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
        // Top face
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
        // Bottom face
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
        // Right face
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1},
        // Left face
        {0, 0},
        {1, 0},
        {1, 1},
        {0, 1}
    });

    mesh.SetIndices({// Front
                     0, 1, 2, 2, 3, 0,
                     // Back
                     4, 5, 6, 6, 7, 4,
                     // Top
                     8, 9, 10, 10, 11, 8,
                     // Bottom
                     12, 13, 14, 14, 15, 12,
                     // Right
                     16, 17, 18, 18, 19, 16,
                     // Left
                     20, 21, 22, 22, 23, 20});

    return mesh;
}

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

Component::Mesh GeneratePlaneMesh(float width, float depth, uint32_t subdivisionsX, uint32_t subdivisionsZ)
{
    Component::Mesh mesh;

    subdivisionsX = std::max(1u, subdivisionsX);
    subdivisionsZ = std::max(1u, subdivisionsZ);

    float halfWidth = width * 0.5f;
    float halfDepth = depth * 0.5f;

    uint32_t vertexCount = (subdivisionsX + 1u) * (subdivisionsZ + 1u);
    mesh.ReserveVertices(vertexCount);
    mesh.ReserveNormals(vertexCount);
    mesh.ReserveTexCoords(vertexCount);
    mesh.ReserveIndices(subdivisionsX * subdivisionsZ * 6u);

    for (uint32_t z = 0u; z <= subdivisionsZ; ++z)
    {
        float zPos = -halfDepth + (static_cast<float>(z) / static_cast<float>(subdivisionsZ)) * depth;
        float v = static_cast<float>(z) / static_cast<float>(subdivisionsZ);

        for (uint32_t x = 0u; x <= subdivisionsX; ++x)
        {
            float xPos = -halfWidth + (static_cast<float>(x) / static_cast<float>(subdivisionsX)) * width;
            float u = static_cast<float>(x) / static_cast<float>(subdivisionsX);

            mesh.EmplaceVertices(xPos, 0.0f, zPos);

            mesh.EmplaceNormals(0.0f, 1.0f, 0.0f);

            mesh.EmplaceTexCoords(u, v);
        }
    }

    for (uint32_t z = 0u; z < subdivisionsZ; ++z)
    {
        for (uint32_t x = 0u; x < subdivisionsX; ++x)
        {
            uint32_t topLeft = z * (subdivisionsX + 1u) + x;
            uint32_t topRight = topLeft + 1u;
            uint32_t bottomLeft = (z + 1u) * (subdivisionsX + 1u) + x;
            uint32_t bottomRight = bottomLeft + 1u;

            mesh.EmplaceIndices(topLeft);
            mesh.EmplaceIndices(bottomLeft);
            mesh.EmplaceIndices(topRight);

            mesh.EmplaceIndices(topRight);
            mesh.EmplaceIndices(bottomLeft);
            mesh.EmplaceIndices(bottomRight);
        }
    }

    return mesh;
}

Component::Mesh GenerateCylinderMesh(float radiusTop, float radiusBottom, float height, uint32_t segments,
                                     uint32_t heightSegments)
{
    Component::Mesh mesh;

    segments = std::max(3u, segments);
    heightSegments = std::max(1u, heightSegments);

    float halfHeight = height * 0.5f;

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

            glm::vec3 normal = glm::normalize(glm::vec3(cosTheta, 0.0f, sinTheta));
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
            mesh.EmplaceIndices(centerTop + seg + 1u);
            mesh.EmplaceIndices(centerTop + seg + 2u);
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
        mesh.EmplaceIndices(centerBottom + seg + 2u);
        mesh.EmplaceIndices(centerBottom + seg + 1u);
    }

    return mesh;
}

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

//=============================================================================
// Soft Body Mesh Generators
//=============================================================================

Component::Mesh GenerateClothMesh(uint32_t width, uint32_t height, float spacing)
{
    Component::Mesh mesh;

    // Need at least a 2x2 grid to form triangles
    if (width < 2u || height < 2u)
    {
        // Still create any vertices (0 or 1 sized grid) but do not generate faces
        mesh.ReserveVertices(static_cast<size_t>(width) * height);
        mesh.ReserveNormals(static_cast<size_t>(width) * height);
        mesh.ReserveTexCoords(static_cast<size_t>(width) * height);

        for (uint32_t y = 0u; y < height; ++y)
        {
            for (uint32_t x = 0u; x < width; ++x)
            {
                mesh.EmplaceVertices(static_cast<float>(x) * spacing, static_cast<float>(y) * spacing, 0.0f);
                mesh.EmplaceNormals(0.0f, 0.0f, 1.0f); // Face +Z
                float uDen = (width > 1u) ? static_cast<float>(width - 1u) : 1.0f;
                float vDen = (height > 1u) ? static_cast<float>(height - 1u) : 1.0f;
                mesh.EmplaceTexCoords(static_cast<float>(x) / uDen, static_cast<float>(y) / vDen);
            }
        }
        return mesh;
    }

    // Generate vertices in XY plane
    mesh.ReserveVertices(static_cast<size_t>(width) * height);
    mesh.ReserveNormals(static_cast<size_t>(width) * height);
    mesh.ReserveTexCoords(static_cast<size_t>(width) * height);

    for (uint32_t y = 0u; y < height; ++y)
    {
        for (uint32_t x = 0u; x < width; ++x)
        {
            mesh.EmplaceVertices(static_cast<float>(x) * spacing, static_cast<float>(y) * spacing, 0.0f);
            mesh.EmplaceNormals(0.0f, 0.0f, 1.0f); // Face +Z
            float u = static_cast<float>(x) / static_cast<float>(width - 1u);
            float v = static_cast<float>(y) / static_cast<float>(height - 1u);
            mesh.EmplaceTexCoords(u, v);
        }
    }

    // Generate faces (two triangles per quad)
    mesh.ReserveIndices(static_cast<size_t>(width - 1u) * (height - 1u) * 6u);
    for (uint32_t y = 0u; y < height - 1u; ++y)
    {
        for (uint32_t x = 0u; x < width - 1u; ++x)
        {
            uint32_t topLeft = y * width + x;
            uint32_t topRight = topLeft + 1u;
            uint32_t bottomLeft = (y + 1u) * width + x;
            uint32_t bottomRight = bottomLeft + 1u;

            // CW winding to match renderer convention
            mesh.EmplaceIndices(topLeft);
            mesh.EmplaceIndices(topRight);
            mesh.EmplaceIndices(bottomLeft);
            mesh.EmplaceIndices(topRight);
            mesh.EmplaceIndices(bottomRight);
            mesh.EmplaceIndices(bottomLeft);
        }
    }

    return mesh;
}

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

Component::Mesh GenerateJellyCubeMesh(uint32_t gridSize, float spacing)
{
    Component::Mesh mesh;

    if (gridSize < 2u)
    {
        if (gridSize == 1u)
        {
            mesh.EmplaceVertices(0.0f, 0.0f, 0.0f);
            mesh.EmplaceNormals(0.0f, 1.0f, 0.0f);
            mesh.EmplaceTexCoords(0.0f, 0.0f);
        }
        return mesh;
    }

    size_t totalVertices = static_cast<size_t>(gridSize) * gridSize * gridSize;
    mesh.ReserveVertices(totalVertices);
    mesh.ReserveNormals(totalVertices);
    mesh.ReserveTexCoords(totalVertices);

    for (uint32_t z = 0u; z < gridSize; ++z)
    {
        for (uint32_t y = 0u; y < gridSize; ++y)
        {
            for (uint32_t x = 0u; x < gridSize; ++x)
            {
                mesh.EmplaceVertices(static_cast<float>(x) * spacing, static_cast<float>(y) * spacing,
                                     static_cast<float>(z) * spacing);
                mesh.EmplaceNormals(0.0f, 1.0f, 0.0f);
                float u = static_cast<float>(x) / static_cast<float>(gridSize - 1u);
                float v = static_cast<float>(y) / static_cast<float>(gridSize - 1u);
                mesh.EmplaceTexCoords(u, v);
            }
        }
    }

    auto getIndex = [gridSize](uint32_t x, uint32_t y, uint32_t z) {
        return z * gridSize * gridSize + y * gridSize + x;
    };

    auto addFace = [&mesh](uint32_t tl, uint32_t tr, uint32_t bl, uint32_t br) {
        mesh.EmplaceIndices(tl);
        mesh.EmplaceIndices(bl);
        mesh.EmplaceIndices(tr);
        mesh.EmplaceIndices(tr);
        mesh.EmplaceIndices(bl);
        mesh.EmplaceIndices(br);
    };

    auto getQuadIndices = [&](uint32_t axis, uint32_t coord, uint32_t i, uint32_t j) -> std::array<uint32_t, 4> {
        uint32_t tl = getIndex(coord, i, j);
        uint32_t tr = getIndex(coord, i + 1u, j);
        uint32_t bl = getIndex(coord, i, j + 1u);
        uint32_t br = getIndex(coord, i + 1u, j + 1u);
        if (axis == 1u) // Y-axis face
        {
            tl = getIndex(i, coord, j);
            tr = getIndex(i + 1u, coord, j);
            bl = getIndex(i, coord, j + 1u);
            br = getIndex(i + 1u, coord, j + 1u);
        }
        else if (axis != 0u) // Z-axis face
        {
            tl = getIndex(i, j, coord);
            tr = getIndex(i + 1u, j, coord);
            bl = getIndex(i, j + 1u, coord);
            br = getIndex(i + 1u, j + 1u, coord);
        }
        return {tl, tr, bl, br};
    };

    auto generateFace = [&](uint32_t axis, uint32_t coord, bool reversed) {
        for (uint32_t i = 0u; i < gridSize - 1u; ++i)
        {
            for (uint32_t j = 0u; j < gridSize - 1u; ++j)
            {
                auto [tl, tr, bl, br] = getQuadIndices(axis, coord, i, j);
                if (reversed)
                {
                    addFace(tl, bl, tr, br);
                }
                else
                {
                    addFace(tl, tr, bl, br);
                }
            }
        }
    };

    generateFace(0u, 0u, false);            // Front (X=0)
    generateFace(0u, gridSize - 1u, true);  // Back (X=gridSize-1)
    generateFace(1u, 0u, true);             // Left (Y=0)
    generateFace(1u, gridSize - 1u, false); // Right (Y=gridSize-1)
    generateFace(2u, 0u, true);             // Bottom (Z=0)
    generateFace(2u, gridSize - 1u, false); // Top (Z=gridSize-1)

    return mesh;
}

Component::Mesh GenerateWheelMesh(float radius, float width, uint32_t segments)
{
    // Generate a cylinder mesh oriented along the Y axis, then rotate vertices
    // so the wheel's axis of rotation is along the X axis (left-right)
    Component::Mesh cylinderMesh = GenerateCylinderMesh(radius, radius, width, segments, 1u);

    Component::Mesh mesh;
    mesh.ReserveVertices(cylinderMesh.GetVertices().size());
    mesh.ReserveNormals(cylinderMesh.GetNormals().size());
    mesh.SetTexCoords(cylinderMesh.GetTexCoords());
    mesh.SetIndices(cylinderMesh.GetIndices());

    // Rotate -90 degrees around Z axis: (x, y, z) -> (y, -x, z)
    // This transforms Y-up cylinder to X-axis aligned wheel
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
