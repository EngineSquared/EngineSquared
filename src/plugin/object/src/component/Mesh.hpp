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
 * When mesh data (vertices, normals, texCoords) is modified, call MarkDirty()
 * to signal that the GPU buffer needs to be updated.
 */
struct Mesh {
    std::vector<glm::vec3> vertices{};
    std::vector<glm::vec3> normals{};
    std::vector<glm::vec2> texCoords{};
    std::vector<uint32_t> indices{};

    explicit Mesh() = default;
    ~Mesh() = default;

    // Move constructor
    Mesh(Mesh &&other) noexcept
        : vertices(std::move(other.vertices)), normals(std::move(other.normals)), texCoords(std::move(other.texCoords)),
          indices(std::move(other.indices)), _dirty(other._dirty)
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
            _dirty = other._dirty;
            other._dirty = false;
        }
        return *this;
    }

    // Copy constructor
    Mesh(const Mesh &mesh) = default;
    // Copy assignment operator
    Mesh &operator=(const Mesh &other) = default;

    /**
     * @brief Mark the mesh as dirty, indicating GPU buffer needs update.
     *
     * Call this method after modifying vertices, normals, or texCoords
     * to ensure the GPU buffer is synchronized on the next render frame.
     *
     * @note This method is const because _dirty is mutable, allowing
     *       modification tracking even on const mesh references.
     */
    void MarkDirty() const { _dirty = true; }

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
