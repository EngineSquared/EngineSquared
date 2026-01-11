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

#include "component/Mesh.hpp"
#include "component/SoftBody.hpp"

#include <set>
#include <stdexcept>

namespace Physics::Component {

//=============================================================================
// Helper: Generate edges from face indices
//
// Note: This function expects triangle-indexed input (3 indices per triangle).
// If the input size is not a multiple of 3, this is treated as invalid input.
//=============================================================================

static std::vector<std::pair<uint32_t, uint32_t>> GenerateEdgesFromFaces(const std::vector<uint32_t> &faceIndices)
{
    // Empty input -> no edges
    if (faceIndices.empty())
        return {};

    // Validate triangle-indexed input
    if (faceIndices.size() % 3u != 0u)
    {
        throw std::invalid_argument(
            "GenerateEdgesFromFaces: faceIndices.size() must be a multiple of 3 (triangle indices)");
    }

    std::set<std::pair<uint32_t, uint32_t>> edgeSet;
    for (size_t i = 0u; i + 2u < faceIndices.size(); i += 3u)
    {
        uint32_t v0 = faceIndices[i];
        uint32_t v1 = faceIndices[i + 1u];
        uint32_t v2 = faceIndices[i + 2u];

        auto addEdge = [&edgeSet](uint32_t a, uint32_t b) {
            if (a > b)
                std::swap(a, b);
            edgeSet.insert({a, b});
        };

        addEdge(v0, v1);
        addEdge(v1, v2);
        addEdge(v2, v0);
    }

    std::vector<std::pair<uint32_t, uint32_t>> edges;
    edges.reserve(edgeSet.size());
    for (const auto &edge : edgeSet)
    {
        edges.push_back(edge);
    }
    return edges;
}

//=============================================================================
// CreateFromMesh
//=============================================================================

SoftBody SoftBody::CreateFromMesh(const Object::Component::Mesh &mesh, const SoftBodySettings &settings)
{
    SoftBody body;
    body.type = SoftBodyType::Custom;
    body.settings = settings;

    // Validate triangle-indexed input for faces.
    // GenerateEdgesFromFaces expects triangles (3 indices per face) and will throw
    // for malformed input; validate early to make the precondition explicit.
    if (!mesh.indices.empty() && mesh.indices.size() % 3u != 0u)
    {
        throw std::invalid_argument(
            "SoftBody::CreateFromMesh: mesh.indices.size() must be a multiple of 3 (triangle indices)");
    }

    // Initialize physics data for each vertex
    body.invMasses.resize(mesh.vertices.size(), 1.0f);
    body.edges = GenerateEdgesFromFaces(mesh.indices);

    return body;
}

//=============================================================================
// CreateCloth
//=============================================================================

std::pair<Object::Component::Mesh, SoftBody> SoftBody::CreateCloth(uint32_t width, uint32_t height, float spacing,
                                                                   float stiffness)
{
    Object::Component::Mesh mesh;
    SoftBody body;

    body.type = SoftBodyType::Cloth;
    body.settings = SoftBodySettings::Cloth(stiffness);

    // Need at least a 2x2 grid to form triangles; handle degenerate sizes safely.
    if (width < 2u || height < 2u)
    {
        // Still create any vertices (0 or 1 sized grid) but do not generate faces/edges.
        mesh.vertices.reserve(width * height);
        mesh.normals.reserve(width * height);
        mesh.texCoords.reserve(width * height);
        body.invMasses.reserve(width * height);

        for (uint32_t y = 0u; y < height; ++y)
        {
            for (uint32_t x = 0u; x < width; ++x)
            {
                mesh.vertices.emplace_back(static_cast<float>(x) * spacing, static_cast<float>(y) * spacing, 0.0f);
                mesh.normals.emplace_back(0.0f, 0.0f, 1.0f); // Face +Z
                float uDen = (width > 1u) ? static_cast<float>(width - 1u) : 1.0f;
                float vDen = (height > 1u) ? static_cast<float>(height - 1u) : 1.0f;
                float u = static_cast<float>(x) / uDen;
                float v = static_cast<float>(y) / vDen;
                mesh.texCoords.emplace_back(u, v);
                body.invMasses.push_back(1.0f);
            }
        }

        // Degenerate cloth: no faces/edges to generate.
        return {std::move(mesh), std::move(body)};
    }

    // Generate vertices in XY plane
    mesh.vertices.reserve(width * height);
    mesh.normals.reserve(width * height);
    mesh.texCoords.reserve(width * height);
    body.invMasses.reserve(width * height);

    for (uint32_t y = 0u; y < height; ++y)
    {
        for (uint32_t x = 0u; x < width; ++x)
        {
            mesh.vertices.emplace_back(static_cast<float>(x) * spacing, static_cast<float>(y) * spacing, 0.0f);
            mesh.normals.emplace_back(0.0f, 0.0f, 1.0f); // Face +Z
            float uDen = (width > 1u) ? static_cast<float>(width - 1u) : 1.0f;
            float vDen = (height > 1u) ? static_cast<float>(height - 1u) : 1.0f;
            float u = static_cast<float>(x) / uDen;
            float v = static_cast<float>(y) / vDen;
            mesh.texCoords.emplace_back(u, v);
            body.invMasses.push_back(1.0f);
        }
    }

    // Generate faces (two triangles per quad)
    mesh.indices.reserve((width - 1u) * (height - 1u) * 6u);
    for (uint32_t y = 0u; y < height - 1u; ++y)
    {
        for (uint32_t x = 0u; x < width - 1u; ++x)
        {
            uint32_t topLeft = y * width + x;
            uint32_t topRight = topLeft + 1u;
            uint32_t bottomLeft = (y + 1u) * width + x;
            uint32_t bottomRight = bottomLeft + 1u;

            // Use CW winding (topLeft, topRight, bottomLeft) to match renderer convention.
            mesh.indices.push_back(topLeft);
            mesh.indices.push_back(topRight);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(topRight);
            mesh.indices.push_back(bottomRight);
            mesh.indices.push_back(bottomLeft);
        }
    }

    body.edges = GenerateEdgesFromFaces(mesh.indices);

    return {std::move(mesh), std::move(body)};
}

//=============================================================================
// CreateRope
//=============================================================================

std::pair<Object::Component::Mesh, SoftBody> SoftBody::CreateRope(uint32_t segmentCount, float segmentLength,
                                                                  float stiffness)
{
    Object::Component::Mesh mesh;
    SoftBody body;

    body.type = SoftBodyType::Rope;
    body.settings = SoftBodySettings::Rope(stiffness);

    if (segmentCount == 0u)
    {
        // Single point rope.
        mesh.vertices.emplace_back(0.0f, 0.0f, 0.0f);
        mesh.normals.emplace_back(0.0f, 0.0f, 1.0f);
        mesh.texCoords.emplace_back(0.0f, 0.0f);
        body.invMasses.push_back(1.0f);
        return {std::move(mesh), std::move(body)};
    }

    uint32_t vertexCount = segmentCount + 1u;
    mesh.vertices.reserve(vertexCount);
    mesh.normals.reserve(vertexCount);
    mesh.texCoords.reserve(vertexCount);
    body.invMasses.reserve(vertexCount);

    for (uint32_t i = 0u; i < vertexCount; ++i)
    {
        mesh.vertices.emplace_back(0.0f, -static_cast<float>(i) * segmentLength, 0.0f);
        mesh.normals.emplace_back(0.0f, 0.0f, 1.0f);
        mesh.texCoords.emplace_back(0.0f, static_cast<float>(i) / static_cast<float>(segmentCount));
        body.invMasses.push_back(1.0f);
    }

    // Edge constraints between consecutive vertices
    body.edges.reserve(segmentCount);
    for (uint32_t i = 0u; i < segmentCount; ++i)
    {
        body.edges.emplace_back(i, i + 1u);
    }

    // No faces for rope (line rendering)

    return {std::move(mesh), std::move(body)};
}

//=============================================================================
// CreateCube
//=============================================================================

std::pair<Object::Component::Mesh, SoftBody> SoftBody::CreateCube(uint32_t gridSize, float spacing)
{
    Object::Component::Mesh mesh;
    SoftBody body;

    body.type = SoftBodyType::Cube;
    body.settings = SoftBodySettings::Default();

    // Need at least 2 points per axis to form surface quads/triangles.
    if (gridSize < 2u)
    {
        // For a 1x1x1 grid create a single vertex; for 0 leave mesh empty.
        if (gridSize == 1u)
        {
            mesh.vertices.emplace_back(0.0f, 0.0f, 0.0f);
            mesh.normals.emplace_back(0.0f, 1.0f, 0.0f);
            mesh.texCoords.emplace_back(0.0f, 0.0f);
            body.invMasses.push_back(1.0f);
        }
        return {std::move(mesh), std::move(body)};
    }

    // Generate vertices in 3D grid
    mesh.vertices.reserve(static_cast<size_t>(gridSize) * gridSize * gridSize);
    mesh.normals.reserve(static_cast<size_t>(gridSize) * gridSize * gridSize);
    mesh.texCoords.reserve(static_cast<size_t>(gridSize) * gridSize * gridSize);
    body.invMasses.reserve(static_cast<size_t>(gridSize) * gridSize * gridSize);

    for (uint32_t z = 0u; z < gridSize; ++z)
    {
        for (uint32_t y = 0u; y < gridSize; ++y)
        {
            for (uint32_t x = 0u; x < gridSize; ++x)
            {
                mesh.vertices.emplace_back(static_cast<float>(x) * spacing, static_cast<float>(y) * spacing,
                                           static_cast<float>(z) * spacing);
                mesh.normals.emplace_back(0.0f, 1.0f, 0.0f); // Placeholder normal
                // Simple UV mapping based on X and Y
                float u = (gridSize > 1u) ? static_cast<float>(x) / (gridSize - 1u) : 0.0f;
                float v = (gridSize > 1u) ? static_cast<float>(y) / (gridSize - 1u) : 0.0f;
                mesh.texCoords.emplace_back(u, v);
                body.invMasses.push_back(1.0f);
            }
        }
    }

    auto getIndex = [gridSize](uint32_t x, uint32_t y, uint32_t z) {
        return z * gridSize * gridSize + y * gridSize + x;
    };

    // Generate edges for structural integrity
    for (uint32_t z = 0u; z < gridSize; ++z)
    {
        for (uint32_t y = 0u; y < gridSize; ++y)
        {
            for (uint32_t x = 0u; x < gridSize; ++x)
            {
                uint32_t idx = getIndex(x, y, z);
                if (x < gridSize - 1u)
                    body.edges.emplace_back(idx, getIndex(x + 1u, y, z));
                if (y < gridSize - 1u)
                    body.edges.emplace_back(idx, getIndex(x, y + 1u, z));
                if (z < gridSize - 1u)
                    body.edges.emplace_back(idx, getIndex(x, y, z + 1u));
            }
        }
    }

    // Generate surface faces for rendering
    // Front face (z = 0)
    for (uint32_t y = 0u; y < gridSize - 1u; ++y)
    {
        for (uint32_t x = 0u; x < gridSize - 1u; ++x)
        {
            uint32_t tl = getIndex(x, y, 0u);
            uint32_t tr = getIndex(x + 1u, y, 0u);
            uint32_t bl = getIndex(x, y + 1u, 0u);
            uint32_t br = getIndex(x + 1u, y + 1u, 0u);
            mesh.indices.push_back(tl);
            mesh.indices.push_back(tr);
            mesh.indices.push_back(bl);
            mesh.indices.push_back(tr);
            mesh.indices.push_back(br);
            mesh.indices.push_back(bl);
        }
    }

    // Back face (z = gridSize - 1)
    for (uint32_t y = 0u; y < gridSize - 1u; ++y)
    {
        for (uint32_t x = 0u; x < gridSize - 1u; ++x)
        {
            uint32_t tl = getIndex(x, y, gridSize - 1u);
            uint32_t tr = getIndex(x + 1u, y, gridSize - 1u);
            uint32_t bl = getIndex(x, y + 1u, gridSize - 1u);
            uint32_t br = getIndex(x + 1u, y + 1u, gridSize - 1u);
            // Match front-face CW winding to keep consistent outward-facing normals.
            mesh.indices.push_back(tl);
            mesh.indices.push_back(tr);
            mesh.indices.push_back(bl);
            mesh.indices.push_back(tr);
            mesh.indices.push_back(br);
            mesh.indices.push_back(bl);
        }
    }

    // Left face (x = 0)
    for (uint32_t z = 0u; z < gridSize - 1u; ++z)
    {
        for (uint32_t y = 0u; y < gridSize - 1u; ++y)
        {
            uint32_t tl = getIndex(0u, y, z);
            uint32_t tr = getIndex(0u, y, z + 1u);
            uint32_t bl = getIndex(0u, y + 1u, z);
            uint32_t br = getIndex(0u, y + 1u, z + 1u);
            mesh.indices.push_back(tl);
            mesh.indices.push_back(bl);
            mesh.indices.push_back(tr);
            mesh.indices.push_back(tr);
            mesh.indices.push_back(bl);
            mesh.indices.push_back(br);
        }
    }

    // Right face (x = gridSize - 1)
    for (uint32_t z = 0u; z < gridSize - 1u; ++z)
    {
        for (uint32_t y = 0u; y < gridSize - 1u; ++y)
        {
            uint32_t tl = getIndex(gridSize - 1u, y, z);
            uint32_t tr = getIndex(gridSize - 1u, y, z + 1u);
            uint32_t bl = getIndex(gridSize - 1u, y + 1u, z);
            uint32_t br = getIndex(gridSize - 1u, y + 1u, z + 1u);
            mesh.indices.push_back(tl);
            mesh.indices.push_back(tr);
            mesh.indices.push_back(bl);
            mesh.indices.push_back(tr);
            mesh.indices.push_back(br);
            mesh.indices.push_back(bl);
        }
    }

    // Bottom face (y = 0)
    for (uint32_t z = 0u; z < gridSize - 1u; ++z)
    {
        for (uint32_t x = 0u; x < gridSize - 1u; ++x)
        {
            uint32_t tl = getIndex(x, 0u, z);
            uint32_t tr = getIndex(x + 1u, 0u, z);
            uint32_t bl = getIndex(x, 0u, z + 1u);
            uint32_t br = getIndex(x + 1u, 0u, z + 1u);
            mesh.indices.push_back(tl);
            mesh.indices.push_back(tr);
            mesh.indices.push_back(bl);
            mesh.indices.push_back(tr);
            mesh.indices.push_back(br);
            mesh.indices.push_back(bl);
        }
    }

    // Top face (y = gridSize - 1)
    for (uint32_t z = 0u; z < gridSize - 1u; ++z)
    {
        for (uint32_t x = 0u; x < gridSize - 1u; ++x)
        {
            uint32_t tl = getIndex(x, gridSize - 1u, z);
            uint32_t tr = getIndex(x + 1u, gridSize - 1u, z);
            uint32_t bl = getIndex(x, gridSize - 1u, z + 1u);
            uint32_t br = getIndex(x + 1u, gridSize - 1u, z + 1u);
            mesh.indices.push_back(tl);
            mesh.indices.push_back(bl);
            mesh.indices.push_back(tr);
            mesh.indices.push_back(tr);
            mesh.indices.push_back(bl);
            mesh.indices.push_back(br);
        }
    }

    return {std::move(mesh), std::move(body)};
}

} // namespace Physics::Component
