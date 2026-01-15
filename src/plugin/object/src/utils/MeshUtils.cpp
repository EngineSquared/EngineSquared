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
 * @file MeshUtils.cpp
 * @brief Implementation of mesh utility functions.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-01-15
 **************************************************************************/

#include "utils/MeshUtils.hpp"

#include <glm/geometric.hpp>

namespace Object::Utils {

static void NormalizeVector(glm::vec3 &normal, float epsilon)
{
    const float lengthSq = glm::dot(normal, normal);
    if (lengthSq > epsilon)
    {
        normal = normal / std::sqrt(lengthSq);
    }
    else
    {
        // Default to up vector for degenerate cases
        normal = glm::vec3(0.0f, 1.0f, 0.0f);
    }
}

void RecalculateNormals(Component::Mesh &mesh)
{
    const auto &vertices = mesh.GetVertices();
    const auto &indices = mesh.GetIndices();
    // Guard clause: need vertices and indices
    if (vertices.empty() || indices.empty())
        return;

    // Guard clause: indices must form complete triangles
    if (indices.size() % 3u != 0)
        return;

    // Ensure normals array is properly sized
    if (mesh.GetNormals().size() != vertices.size())
    {
        mesh.SetNormals(std::vector<glm::vec3>(vertices.size()));
    }

    // Initialize all normals to zero
    for (size_t i = 0u; i < vertices.size(); ++i)
    {
        mesh.SetNormalAt(i, glm::vec3(0.0f));
    }

    // Accumulate face normals for each vertex
    const size_t triangleCount = indices.size() / 3u;
    for (size_t i = 0u; i < triangleCount; ++i)
    {
        const uint32_t idx0 = indices[i * 3 + 0];
        const uint32_t idx1 = indices[i * 3 + 1];
        const uint32_t idx2 = indices[i * 3 + 2];

        // Bounds check
        if (idx0 >= vertices.size() || idx1 >= vertices.size() || idx2 >= vertices.size())
            continue;

        const glm::vec3 &v0 = vertices[idx0];
        const glm::vec3 &v1 = vertices[idx1];
        const glm::vec3 &v2 = vertices[idx2];

        // Compute face normal using cross product
        // Edge vectors from v0
        const glm::vec3 edge1 = v1 - v0;
        const glm::vec3 edge2 = v2 - v0;

        // Cross product gives normal direction (not normalized yet)
        // The magnitude is proportional to triangle area, which provides
        // area-weighted averaging when accumulated
        const glm::vec3 faceNormal = glm::cross(edge1, edge2);

        // Accumulate to each vertex of this face
        mesh.SetNormalAt(idx0, normals[idx0] + faceNormal);
        mesh.SetNormalAt(idx1, normals[idx1] + faceNormal);
        mesh.SetNormalAt(idx2, normals[idx2] + faceNormal);
    }

    // Normalize all vertex normals
    constexpr float epsilon = 1e-8f;
    for (size_t i = 0u; i < normals.size(); ++i)
    {
        glm::vec3 normal = normals[i];
        NormalizeVector(normal, epsilon);
        mesh.SetNormalAt(i, normal);
    }
}

bool ValidateMeshArraySizes(const Component::Mesh &mesh)
{
    const size_t vertexCount = mesh.GetVertices().size();
    return mesh.GetNormals().size() == vertexCount && mesh.GetTexCoords().size() == vertexCount;
}

} // namespace Object::Utils
