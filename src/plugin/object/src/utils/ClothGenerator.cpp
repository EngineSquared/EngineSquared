#include "ClothGenerator.hpp"
#include "Object.pch.hpp"

namespace Object::Utils {

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

} // namespace Object::Utils
