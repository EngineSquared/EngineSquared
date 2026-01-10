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

namespace Physics::Component {

//=============================================================================
// Helper: Generate edges from face indices
//=============================================================================

static std::vector<std::pair<uint32_t, uint32_t>> GenerateEdgesFromFaces(const std::vector<uint32_t> &faceIndices)
{
    std::set<std::pair<uint32_t, uint32_t>> edgeSet;
    for (size_t i = 0; i + 2 < faceIndices.size(); i += 3)
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

    // Generate vertices in XY plane
    mesh.vertices.reserve(width * height);
    mesh.normals.reserve(width * height);
    mesh.texCoords.reserve(width * height);
    body.invMasses.reserve(width * height);

    for (uint32_t y = 0; y < height; ++y)
    {
        for (uint32_t x = 0; x < width; ++x)
        {
            mesh.vertices.emplace_back(static_cast<float>(x) * spacing, static_cast<float>(y) * spacing, 0.0f);
            mesh.normals.emplace_back(0.0f, 0.0f, 1.0f); // Face +Z
            float uDen = (width > 1) ? static_cast<float>(width - 1) : 1.0f;
            float vDen = (height > 1) ? static_cast<float>(height - 1) : 1.0f;
            float u = static_cast<float>(x) / uDen;
            float v = static_cast<float>(y) / vDen;
            mesh.texCoords.emplace_back(u, v);
            body.invMasses.push_back(1.0f);
        }
    }

    // Generate faces (two triangles per quad)
    mesh.indices.reserve((width - 1) * (height - 1) * 6);
    for (uint32_t y = 0; y < height - 1; ++y)
    {
        for (uint32_t x = 0; x < width - 1; ++x)
        {
            uint32_t topLeft = y * width + x;
            uint32_t topRight = topLeft + 1;
            uint32_t bottomLeft = (y + 1) * width + x;
            uint32_t bottomRight = bottomLeft + 1;

            mesh.indices.push_back(topLeft);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(topRight);
            mesh.indices.push_back(topRight);
            mesh.indices.push_back(bottomLeft);
            mesh.indices.push_back(bottomRight);
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

    uint32_t vertexCount = segmentCount + 1;
    mesh.vertices.reserve(vertexCount);
    mesh.normals.reserve(vertexCount);
    mesh.texCoords.reserve(vertexCount);
    body.invMasses.reserve(vertexCount);

    for (uint32_t i = 0; i < vertexCount; ++i)
    {
        mesh.vertices.emplace_back(0.0f, -static_cast<float>(i) * segmentLength, 0.0f);
        mesh.normals.emplace_back(0.0f, 0.0f, 1.0f);
        mesh.texCoords.emplace_back(0.0f, static_cast<float>(i) / segmentCount);
        body.invMasses.push_back(1.0f);
    }

    // Edge constraints between consecutive vertices
    body.edges.reserve(segmentCount);
    for (uint32_t i = 0; i < segmentCount; ++i)
    {
        body.edges.emplace_back(i, i + 1);
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

    // Generate vertices in 3D grid
    mesh.vertices.reserve(gridSize * gridSize * gridSize);
    mesh.normals.reserve(gridSize * gridSize * gridSize);
    mesh.texCoords.reserve(gridSize * gridSize * gridSize);
    body.invMasses.reserve(gridSize * gridSize * gridSize);

    for (uint32_t z = 0; z < gridSize; ++z)
    {
        for (uint32_t y = 0; y < gridSize; ++y)
        {
            for (uint32_t x = 0; x < gridSize; ++x)
            {
                mesh.vertices.emplace_back(static_cast<float>(x) * spacing, static_cast<float>(y) * spacing,
                                           static_cast<float>(z) * spacing);
                mesh.normals.emplace_back(0.0f, 1.0f, 0.0f); // Placeholder normal
                // Simple UV mapping based on X and Y
                float u = (gridSize > 1) ? static_cast<float>(x) / (gridSize - 1) : 0.0f;
                float v = (gridSize > 1) ? static_cast<float>(y) / (gridSize - 1) : 0.0f;
                mesh.texCoords.emplace_back(u, v);
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
                if (x < gridSize - 1)
                    body.edges.emplace_back(idx, getIndex(x + 1, y, z));
                if (y < gridSize - 1)
                    body.edges.emplace_back(idx, getIndex(x, y + 1, z));
                if (z < gridSize - 1)
                    body.edges.emplace_back(idx, getIndex(x, y, z + 1));
            }
        }
    }

    // Generate surface faces for rendering
    // Front face (z = 0)
    for (uint32_t y = 0; y < gridSize - 1; ++y)
    {
        for (uint32_t x = 0; x < gridSize - 1; ++x)
        {
            uint32_t tl = getIndex(x, y, 0);
            uint32_t tr = getIndex(x + 1, y, 0);
            uint32_t bl = getIndex(x, y + 1, 0);
            uint32_t br = getIndex(x + 1, y + 1, 0);
            mesh.indices.push_back(tl);
            mesh.indices.push_back(tr);
            mesh.indices.push_back(bl);
            mesh.indices.push_back(tr);
            mesh.indices.push_back(br);
            mesh.indices.push_back(bl);
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
