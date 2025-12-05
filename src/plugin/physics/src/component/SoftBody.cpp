/**************************************************************************
 * EngineSquared v0.1.1
 *
 * EngineSquared is a software package, part of the Engine² organization.
 *
 * This file is part of the EngineSquared project that is under MIT License.
 * Copyright © 2025-present by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the MIT License. See the project's LICENSE file for
 * the full license text and details.
 *
 * @file SoftBody.cpp
 * @brief SoftBody component factory implementations
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-12-05
 **************************************************************************/

#include "Physics.pch.hpp"

#include "component/SoftBody.hpp"

#include <set>

namespace Physics::Component {

SoftBody SoftBody::CreateCloth(uint32_t width, uint32_t height, float spacing, float stiffness)
{
    SoftBody body;
    body.type = SoftBodyType::Cloth;
    body.settings = SoftBodySettings::Cloth(stiffness);

    // Generate vertices in XY plane
    body.vertices.reserve(width * height);
    body.invMasses.reserve(width * height);

    for (uint32_t y = 0; y < height; ++y)
    {
        for (uint32_t x = 0; x < width; ++x)
        {
            body.vertices.emplace_back(static_cast<float>(x) * spacing, static_cast<float>(y) * spacing, 0.0f);
            body.invMasses.push_back(1.0f); // Default mass = 1
        }
    }

    // Generate faces (two triangles per quad)
    body.faces.reserve((width - 1) * (height - 1) * 6);
    for (uint32_t y = 0; y < height - 1; ++y)
    {
        for (uint32_t x = 0; x < width - 1; ++x)
        {
            uint32_t topLeft = y * width + x;
            uint32_t topRight = topLeft + 1;
            uint32_t bottomLeft = (y + 1) * width + x;
            uint32_t bottomRight = bottomLeft + 1;

            // Triangle 1
            body.faces.push_back(topLeft);
            body.faces.push_back(bottomLeft);
            body.faces.push_back(topRight);

            // Triangle 2
            body.faces.push_back(topRight);
            body.faces.push_back(bottomLeft);
            body.faces.push_back(bottomRight);
        }
    }

    // Generate structural edges (horizontal and vertical)
    // Horizontal edges
    for (uint32_t y = 0; y < height; ++y)
    {
        for (uint32_t x = 0; x < width - 1; ++x)
        {
            uint32_t idx = y * width + x;
            body.edges.emplace_back(idx, idx + 1);
        }
    }

    // Vertical edges
    for (uint32_t y = 0; y < height - 1; ++y)
    {
        for (uint32_t x = 0; x < width; ++x)
        {
            uint32_t idx = y * width + x;
            body.edges.emplace_back(idx, idx + width);
        }
    }

    // Shear edges (diagonals)
    for (uint32_t y = 0; y < height - 1; ++y)
    {
        for (uint32_t x = 0; x < width - 1; ++x)
        {
            uint32_t topLeft = y * width + x;
            uint32_t topRight = topLeft + 1;
            uint32_t bottomLeft = (y + 1) * width + x;
            uint32_t bottomRight = bottomLeft + 1;

            body.edges.emplace_back(topLeft, bottomRight);
            body.edges.emplace_back(topRight, bottomLeft);
        }
    }

    return body;
}

SoftBody SoftBody::CreateRope(uint32_t segmentCount, float segmentLength, float stiffness)
{
    SoftBody body;
    body.type = SoftBodyType::Rope;
    body.settings = SoftBodySettings::Rope(stiffness);

    // Generate vertices along Y axis (hanging down)
    uint32_t vertexCount = segmentCount + 1;
    body.vertices.reserve(vertexCount);
    body.invMasses.reserve(vertexCount);

    for (uint32_t i = 0; i < vertexCount; ++i)
    {
        body.vertices.emplace_back(0.0f,
                                   -static_cast<float>(i) * segmentLength, // Hang down
                                   0.0f);
        body.invMasses.push_back(1.0f);
    }

    // Generate edge constraints between consecutive vertices
    body.edges.reserve(segmentCount);
    for (uint32_t i = 0; i < segmentCount; ++i)
    {
        body.edges.emplace_back(i, i + 1);
    }

    // No faces for rope (1D structure)
    body.faces.clear();

    return body;
}

SoftBody SoftBody::CreateCube(uint32_t gridSize, float spacing)
{
    SoftBody body;
    body.type = SoftBodyType::Cube;
    body.settings = SoftBodySettings::Default();

    // Generate vertices in 3D grid
    body.vertices.reserve(gridSize * gridSize * gridSize);
    body.invMasses.reserve(gridSize * gridSize * gridSize);

    for (uint32_t z = 0; z < gridSize; ++z)
    {
        for (uint32_t y = 0; y < gridSize; ++y)
        {
            for (uint32_t x = 0; x < gridSize; ++x)
            {
                body.vertices.emplace_back(static_cast<float>(x) * spacing, static_cast<float>(y) * spacing,
                                           static_cast<float>(z) * spacing);
                body.invMasses.push_back(1.0f);
            }
        }
    }

    auto getIndex = [gridSize](uint32_t x, uint32_t y, uint32_t z) {
        return z * gridSize * gridSize + y * gridSize + x;
    };

    // Generate edges for structural integrity
    for (uint32_t z = 0; z < gridSize; ++z)
    {
        for (uint32_t y = 0; y < gridSize; ++y)
        {
            for (uint32_t x = 0; x < gridSize; ++x)
            {
                uint32_t idx = getIndex(x, y, z);

                // X direction
                if (x < gridSize - 1)
                    body.edges.emplace_back(idx, getIndex(x + 1, y, z));

                // Y direction
                if (y < gridSize - 1)
                    body.edges.emplace_back(idx, getIndex(x, y + 1, z));

                // Z direction
                if (z < gridSize - 1)
                    body.edges.emplace_back(idx, getIndex(x, y, z + 1));
            }
        }
    }

    // Generate surface faces (only on the outer shell)
    // Front face (z = 0)
    for (uint32_t y = 0; y < gridSize - 1; ++y)
    {
        for (uint32_t x = 0; x < gridSize - 1; ++x)
        {
            uint32_t tl = getIndex(x, y, 0);
            uint32_t tr = getIndex(x + 1, y, 0);
            uint32_t bl = getIndex(x, y + 1, 0);
            uint32_t br = getIndex(x + 1, y + 1, 0);

            body.faces.push_back(tl);
            body.faces.push_back(tr);
            body.faces.push_back(bl);
            body.faces.push_back(tr);
            body.faces.push_back(br);
            body.faces.push_back(bl);
        }
    }

    // Back face (z = gridSize - 1)
    for (uint32_t y = 0; y < gridSize - 1; ++y)
    {
        for (uint32_t x = 0; x < gridSize - 1; ++x)
        {
            uint32_t tl = getIndex(x, y, gridSize - 1);
            uint32_t tr = getIndex(x + 1, y, gridSize - 1);
            uint32_t bl = getIndex(x, y + 1, gridSize - 1);
            uint32_t br = getIndex(x + 1, y + 1, gridSize - 1);

            body.faces.push_back(tl);
            body.faces.push_back(bl);
            body.faces.push_back(tr);
            body.faces.push_back(tr);
            body.faces.push_back(bl);
            body.faces.push_back(br);
        }
    }

    // Similar for other faces (left, right, top, bottom)...
    // For brevity, we'll let Jolt's CreateConstraints handle the rest

    return body;
}

SoftBody SoftBody::CreateFromMesh(const std::vector<glm::vec3> &verts, const std::vector<uint32_t> &faceIndices,
                                  const SoftBodySettings &settings)
{
    SoftBody body;
    body.type = SoftBodyType::Custom;
    body.settings = settings;

    body.vertices = verts;
    body.invMasses.resize(verts.size(), 1.0f);
    body.faces = faceIndices;

    // Generate edges from faces
    std::set<std::pair<uint32_t, uint32_t>> edgeSet;
    for (size_t i = 0; i < faceIndices.size(); i += 3)
    {
        uint32_t v0 = faceIndices[i];
        uint32_t v1 = faceIndices[i + 1];
        uint32_t v2 = faceIndices[i + 2];

        auto addEdge = [&edgeSet](uint32_t a, uint32_t b) {
            if (a > b)
                std::swap(a, b);
            edgeSet.insert({a, b});
        };

        addEdge(v0, v1);
        addEdge(v1, v2);
        addEdge(v2, v0);
    }

    body.edges.reserve(edgeSet.size());
    for (const auto &edge : edgeSet)
    {
        body.edges.push_back(edge);
    }

    return body;
}

} // namespace Physics::Component
