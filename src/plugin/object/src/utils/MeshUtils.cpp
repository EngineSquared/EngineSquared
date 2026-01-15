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

void RecalculateNormals(Component::Mesh &mesh)
{
    // Guard clause: need vertices and indices
    if (mesh.vertices.empty() || mesh.indices.empty())
        return;

    // Guard clause: indices must form complete triangles
    if (mesh.indices.size() % 3 != 0)
        return;

    // Ensure normals array is properly sized
    if (mesh.normals.size() != mesh.vertices.size())
    {
        mesh.normals.resize(mesh.vertices.size());
    }

    // Initialize all normals to zero
    for (auto &normal : mesh.normals)
    {
        normal = glm::vec3(0.0f);
    }

    // Accumulate face normals for each vertex
    const size_t triangleCount = mesh.indices.size() / 3;
    for (size_t i = 0; i < triangleCount; ++i)
    {
        const uint32_t idx0 = mesh.indices[i * 3 + 0];
        const uint32_t idx1 = mesh.indices[i * 3 + 1];
        const uint32_t idx2 = mesh.indices[i * 3 + 2];

        // Bounds check
        if (idx0 >= mesh.vertices.size() || idx1 >= mesh.vertices.size() || idx2 >= mesh.vertices.size())
            continue;

        const glm::vec3 &v0 = mesh.vertices[idx0];
        const glm::vec3 &v1 = mesh.vertices[idx1];
        const glm::vec3 &v2 = mesh.vertices[idx2];

        // Compute face normal using cross product
        // Edge vectors from v0
        const glm::vec3 edge1 = v1 - v0;
        const glm::vec3 edge2 = v2 - v0;

        // Cross product gives normal direction (not normalized yet)
        // The magnitude is proportional to triangle area, which provides
        // area-weighted averaging when accumulated
        const glm::vec3 faceNormal = glm::cross(edge1, edge2);

        // Accumulate to each vertex of this face
        mesh.normals[idx0] += faceNormal;
        mesh.normals[idx1] += faceNormal;
        mesh.normals[idx2] += faceNormal;
    }

    // Normalize all vertex normals
    constexpr float epsilon = 1e-8f;
    for (auto &normal : mesh.normals)
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
}

bool ValidateMeshArraySizes(const Component::Mesh &mesh)
{
    const size_t vertexCount = mesh.vertices.size();
    return mesh.normals.size() == vertexCount && mesh.texCoords.size() == vertexCount;
}

} // namespace Object::Utils
