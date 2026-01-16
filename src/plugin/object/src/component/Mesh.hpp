/**************************************************************************
 * EngineSquared v0.1.1
 *
 * EngineSquared is a software package, part of the Engine² organization.
 *
 * This file is part of the EngineSquared project that is under MIT License.
 * Copyright © 2024-present by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the MIT License. See the project's LICENSE file for
 * the full license text and details.
 *
 * @file Mesh.hpp
 * @brief Mesh struct declaration.
 *
 * This struct is used to represent a mesh.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2024-12-06
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Object::Component {

/**
 * @brief Mesh structure.
 *
 * This structure is used to represent a mesh.
 * It contains the vertices and indices of the mesh.
 *
 * The mesh supports a dirty flag mechanism for efficient GPU updates.
 * When mesh data is modified through the setter methods (SetVertices, SetVertexAt, etc.),
 * the dirty flag is automatically set to signal that the GPU buffer needs to be updated.
 */
struct Mesh {
    explicit Mesh() = default;
    ~Mesh()
    {
        vertices.clear();
        normals.clear();
        texCoords.clear();
        indices.clear();
        _dirty = false;
    }

    // Move constructor
    Mesh(Mesh &&other) noexcept
        : vertices(std::move(other.vertices)), normals(std::move(other.normals)), texCoords(std::move(other.texCoords)),
          indices(std::move(other.indices)), _dirty(true)
    {
        other._dirty = false;
    }

    // Move assignment operator
    Mesh &operator=(Mesh &&other) noexcept
    {
        if (this != &other)
        {
            vertices = std::move(other.vertices);
            normals = std::move(other.normals);
            texCoords = std::move(other.texCoords);
            indices = std::move(other.indices);
            _dirty = true;
            other._dirty = false;
        }
        return *this;
    }

    // Copy constructor
    Mesh(const Mesh &other) noexcept
        : vertices(other.vertices), normals(other.normals), texCoords(other.texCoords), indices(other.indices),
          _dirty(true)
    {
    }

    // Copy assignment operator
    Mesh &operator=(const Mesh &other)
    {
        if (this != &other)
        {
            vertices = other.vertices;
            normals = other.normals;
            texCoords = other.texCoords;
            indices = other.indices;
            _dirty = true;
        }
        return *this;
    }

    // Getters
    [[nodiscard]] const std::vector<glm::vec3> &GetVertices() const { return vertices; }
    [[nodiscard]] const std::vector<glm::vec3> &GetNormals() const { return normals; }
    [[nodiscard]] const std::vector<glm::vec2> &GetTexCoords() const { return texCoords; }
    [[nodiscard]] const std::vector<uint32_t> &GetIndices() const { return indices; }

    // Setters

    //---------------- Vertex Methods ----------------//
    void SetVertices(const std::vector<glm::vec3> &newVertices)
    {
        vertices = newVertices;
        _dirty = true;
    }

    void SetVertexAt(size_t index, const glm::vec3 &vertex)
    {
        if (index >= vertices.size())
            return;
        vertices[index] = vertex;
        _dirty = true;
    }

    void ReserveVertices(size_t count) { vertices.reserve(count); }

    template <typename... Args> void EmplaceVertices(Args &&...args)
    {
        vertices.emplace_back(std::forward<Args>(args)...);
        _dirty = true;
    }

    //---------------- Normal Methods ----------------//
    void SetNormals(const std::vector<glm::vec3> &newNormals)
    {
        normals = newNormals;
        _dirty = true;
    }

    void SetNormalAt(size_t index, const glm::vec3 &normal)
    {
        if (index >= normals.size())
            return;
        normals[index] = normal;
        _dirty = true;
    }

    void ReserveNormals(size_t count) { normals.reserve(count); }

    template <typename... Args> void EmplaceNormals(Args &&...args)
    {
        normals.emplace_back(std::forward<Args>(args)...);
        _dirty = true;
    }

    //---------------- TexCoord Methods ----------------//
    void SetTexCoords(const std::vector<glm::vec2> &newTexCoords)
    {
        texCoords = newTexCoords;
        _dirty = true;
    }

    void SetTexCoordAt(size_t index, const glm::vec2 &texCoord)
    {
        if (index >= texCoords.size())
            return;
        texCoords[index] = texCoord;
        _dirty = true;
    }

    void ReserveTexCoords(size_t count) { texCoords.reserve(count); }

    template <typename... Args> void EmplaceTexCoords(Args &&...args)
    {
        texCoords.emplace_back(std::forward<Args>(args)...);
        _dirty = true;
    }

    //---------------- Index Methods ----------------//
    void SetIndices(const std::vector<uint32_t> &newIndices)
    {
        indices = newIndices;
        _dirty = true;
    }

    void SetIndexAt(size_t index, uint32_t indexValue)
    {
        if (index >= indices.size())
            return;
        indices[index] = indexValue;
        _dirty = true;
    }

    void ReserveIndices(size_t count) { indices.reserve(count); }

    template <typename... Args> void EmplaceIndices(Args &&...args)
    {
        indices.emplace_back(std::forward<Args>(args)...);
        _dirty = true;
    }

    /**
     * @brief Check if the mesh data has been modified since last GPU sync.
     *
     * @return true if mesh data was modified and GPU buffer needs update.
     * @return false if mesh data is in sync with GPU buffer.
     */
    [[nodiscard]] bool IsDirty() const { return _dirty; }

    /**
     * @brief Clear the dirty flag after GPU buffer has been updated.
     *
     * Called by the graphics system after successfully updating the GPU buffer
     * to indicate the mesh is now in sync.
     */
    void ClearDirty() const { _dirty = false; }

  private:
    std::vector<glm::vec3> vertices{};
    std::vector<glm::vec3> normals{};
    std::vector<glm::vec2> texCoords{};
    std::vector<uint32_t> indices{};

    /**
     * @brief Dirty flag for GPU synchronization optimization.
     *
     * Mutable to allow modification tracking on const mesh references.
     * When true, indicates that mesh data has changed and the GPU buffer
     * needs to be updated.
     */
    mutable bool _dirty = false;
};
} // namespace Object::Component
