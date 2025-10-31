#include "resource/ShapeGenerator.hpp"
#include "Object.pch.hpp"

#include <cmath>

namespace Object::Resource {

Component::Mesh GenerateCubeMesh(float size)
{
    Component::Mesh mesh;

    float halfSize = size * 0.5f;

    mesh.vertices.reserve(24);
    mesh.normals.reserve(24);
    mesh.texCoords.reserve(24);
    mesh.indices.reserve(36);

    mesh.vertices = {
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
    };

    mesh.normals = {
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
    };

    mesh.texCoords = {
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
    };

    mesh.indices = {// Front
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
                    20, 21, 22, 22, 23, 20};

    return mesh;
}

Component::Mesh GenerateSphereMesh(float radius, uint32_t segments, uint32_t rings)
{
    Component::Mesh mesh;

    // Ensure minimum subdivisions
    segments = std::max(3u, segments);
    rings = std::max(2u, rings);

    uint32_t vertexCount = (rings + 1u) * (segments + 1u);
    mesh.vertices.reserve(vertexCount);
    mesh.normals.reserve(vertexCount);
    mesh.texCoords.reserve(vertexCount);
    mesh.indices.reserve(rings * segments * 6u);

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
            mesh.vertices.emplace_back(vertex);

            glm::vec3 normal = glm::normalize(vertex);
            mesh.normals.emplace_back(normal);

            mesh.texCoords.emplace_back(static_cast<float>(seg) / static_cast<float>(segments),
                                        static_cast<float>(ring) / static_cast<float>(rings));
        }
    }

    for (uint32_t ring = 0u; ring < rings; ++ring)
    {
        for (uint32_t seg = 0u; seg < segments; ++seg)
        {
            uint32_t current = ring * (segments + 1u) + seg;
            uint32_t next = current + segments + 1u;

            mesh.indices.emplace_back(current);
            mesh.indices.emplace_back(next);
            mesh.indices.emplace_back(current + 1u);

            mesh.indices.emplace_back(current + 1u);
            mesh.indices.emplace_back(next);
            mesh.indices.emplace_back(next + 1u);
        }
    }

    return mesh;
}

Component::Mesh GeneratePlaneMesh(float width, float depth, uint32_t subdivisionsX, uint32_t subdivisionsZ)
{
    Component::Mesh mesh;

    // Ensure minimum subdivisions
    subdivisionsX = std::max(1u, subdivisionsX);
    subdivisionsZ = std::max(1u, subdivisionsZ);

    float halfWidth = width * 0.5f;
    float halfDepth = depth * 0.5f;

    uint32_t vertexCount = (subdivisionsX + 1u) * (subdivisionsZ + 1u);
    mesh.vertices.reserve(vertexCount);
    mesh.normals.reserve(vertexCount);
    mesh.texCoords.reserve(vertexCount);
    mesh.indices.reserve(subdivisionsX * subdivisionsZ * 6u);

    for (uint32_t z = 0u; z <= subdivisionsZ; ++z)
    {
        float zPos = -halfDepth + (static_cast<float>(z) / static_cast<float>(subdivisionsZ)) * depth;
        float v = static_cast<float>(z) / static_cast<float>(subdivisionsZ);

        for (uint32_t x = 0u; x <= subdivisionsX; ++x)
        {
            float xPos = -halfWidth + (static_cast<float>(x) / static_cast<float>(subdivisionsX)) * width;
            float u = static_cast<float>(x) / static_cast<float>(subdivisionsX);

            mesh.vertices.emplace_back(xPos, 0.0f, zPos);

            mesh.normals.emplace_back(0.0f, 1.0f, 0.0f);

            mesh.texCoords.emplace_back(u, v);
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

            mesh.indices.emplace_back(topLeft);
            mesh.indices.emplace_back(bottomLeft);
            mesh.indices.emplace_back(topRight);

            mesh.indices.emplace_back(topRight);
            mesh.indices.emplace_back(bottomLeft);
            mesh.indices.emplace_back(bottomRight);
        }
    }

    return mesh;
}

Component::Mesh GenerateCylinderMesh(float radiusTop, float radiusBottom, float height, uint32_t segments,
                                     uint32_t heightSegments)
{
    Component::Mesh mesh;

    // Ensure minimum subdivisions
    segments = std::max(3u, segments);
    heightSegments = std::max(1u, heightSegments);

    float halfHeight = height * 0.5f;

    uint32_t sideVertexCount = (heightSegments + 1u) * (segments + 1u);
    uint32_t capVertexCount = 2u * (segments + 2u); // Top and bottom caps (center + ring)
    mesh.vertices.reserve(sideVertexCount + capVertexCount);
    mesh.normals.reserve(sideVertexCount + capVertexCount);
    mesh.texCoords.reserve(sideVertexCount + capVertexCount);
    mesh.indices.reserve(heightSegments * segments * 6u + segments * 6u); // Sides + caps

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
            mesh.vertices.emplace_back(vertex);

            glm::vec3 normal = glm::normalize(glm::vec3(cosTheta, 0.0f, sinTheta));
            mesh.normals.emplace_back(normal);

            mesh.texCoords.emplace_back(static_cast<float>(seg) / static_cast<float>(segments), t);
        }
    }

    for (uint32_t h = 0u; h < heightSegments; ++h)
    {
        for (uint32_t seg = 0u; seg < segments; ++seg)
        {
            uint32_t current = h * (segments + 1u) + seg;
            uint32_t next = current + segments + 1u;

            mesh.indices.emplace_back(current);
            mesh.indices.emplace_back(next);
            mesh.indices.emplace_back(current + 1u);

            mesh.indices.emplace_back(current + 1u);
            mesh.indices.emplace_back(next);
            mesh.indices.emplace_back(next + 1u);
        }
    }

    // Generate top cap (if radius > 0)
    if (radiusTop > 0.0f)
    {
        auto centerTop = static_cast<uint32_t>(mesh.vertices.size());
        mesh.vertices.emplace_back(0.0f, halfHeight, 0.0f);
        mesh.normals.emplace_back(0.0f, 1.0f, 0.0f);
        mesh.texCoords.emplace_back(0.5f, 0.5f);

        for (uint32_t seg = 0; seg <= segments; ++seg)
        {
            float theta = static_cast<float>(seg) / static_cast<float>(segments) * 2.0f * glm::pi<float>();
            float cosTheta = std::cos(theta);
            float sinTheta = std::sin(theta);

            mesh.vertices.emplace_back(radiusTop * cosTheta, halfHeight, radiusTop * sinTheta);
            mesh.normals.emplace_back(0.0f, 1.0f, 0.0f);
            mesh.texCoords.emplace_back(0.5f + 0.5f * cosTheta, 0.5f + 0.5f * sinTheta);
        }

        for (uint32_t seg = 0u; seg < segments; ++seg)
        {
            mesh.indices.emplace_back(centerTop);
            mesh.indices.emplace_back(centerTop + seg + 1u);
            mesh.indices.emplace_back(centerTop + seg + 2u);
        }
    }

    // Generate bottom cap (if radius > 0)
    if (radiusBottom <= 0.0f)
        return mesh;

    auto centerBottom = static_cast<uint32_t>(mesh.vertices.size());
    mesh.vertices.emplace_back(0.0f, -halfHeight, 0.0f);
    mesh.normals.emplace_back(0.0f, -1.0f, 0.0f);
    mesh.texCoords.emplace_back(0.5f, 0.5f);

    for (uint32_t seg = 0u; seg <= segments; ++seg)
    {
        float theta = static_cast<float>(seg) / static_cast<float>(segments) * 2.0f * glm::pi<float>();
        float cosTheta = std::cos(theta);
        float sinTheta = std::sin(theta);

        mesh.vertices.emplace_back(radiusBottom * cosTheta, -halfHeight, radiusBottom * sinTheta);
        mesh.normals.emplace_back(0.0f, -1.0f, 0.0f);
        mesh.texCoords.emplace_back(0.5f + 0.5f * cosTheta, 0.5f + 0.5f * sinTheta);
    }

    for (uint32_t seg = 0u; seg < segments; ++seg)
    {
        mesh.indices.emplace_back(centerBottom);
        mesh.indices.emplace_back(centerBottom + seg + 2u);
        mesh.indices.emplace_back(centerBottom + seg + 1u);
    }

    return mesh;
}

Engine::Entity CreateCube(Engine::Core &core, float size, const glm::vec3 &position, const glm::quat &rotation,
                          const glm::vec3 &scale)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position, scale, rotation);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = GenerateCubeMesh(size);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreateSphere(Engine::Core &core, float radius, const glm::vec3 &position, uint32_t segments,
                            uint32_t rings)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = GenerateSphereMesh(radius, segments, rings);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreatePlane(Engine::Core &core, float width, float depth, const glm::vec3 &position,
                           uint32_t subdivisionsX, uint32_t subdivisionsZ)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = GeneratePlaneMesh(width, depth, subdivisionsX, subdivisionsZ);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

Engine::Entity CreateCylinder(Engine::Core &core, float radiusTop, float radiusBottom, float height,
                              const glm::vec3 &position, uint32_t segments)
{
    auto entity = core.CreateEntity();

    auto transform = Component::Transform(position);
    entity.AddComponent<Component::Transform>(core, transform);

    auto mesh = GenerateCylinderMesh(radiusTop, radiusBottom, height, segments, 1);
    entity.AddComponent<Component::Mesh>(core, std::move(mesh));

    return entity;
}

} // namespace Object::Resource
