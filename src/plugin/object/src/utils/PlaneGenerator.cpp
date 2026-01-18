#include "PlaneGenerator.hpp"
#include "Object.pch.hpp"

namespace Object::Utils {

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

} // namespace Object::Utils
