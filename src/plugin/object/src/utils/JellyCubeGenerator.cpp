#include "JellyCubeGenerator.hpp"
#include "Object.pch.hpp"

namespace Object::Utils {

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
        switch (axis)
        {
        case 0u:
            return {getIndex(coord, i, j), getIndex(coord, i + 1u, j), getIndex(coord, i, j + 1u),
                    getIndex(coord, i + 1u, j + 1u)};
        case 1u:
            return {getIndex(i, coord, j), getIndex(i + 1u, coord, j), getIndex(i, coord, j + 1u),
                    getIndex(i + 1u, coord, j + 1u)};
        default:
            return {getIndex(i, j, coord), getIndex(i + 1u, j, coord), getIndex(i, j + 1u, coord),
                    getIndex(i + 1u, j + 1u, coord)};
        }
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

    generateFace(0u, 0u, false);
    generateFace(0u, gridSize - 1u, true);
    generateFace(1u, 0u, true);
    generateFace(1u, gridSize - 1u, false);
    generateFace(2u, 0u, true);
    generateFace(2u, gridSize - 1u, false);

    return mesh;
}

} // namespace Object::Utils
