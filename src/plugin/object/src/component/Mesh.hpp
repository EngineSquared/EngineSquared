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

namespace Plugin::Object::Component {

/**
 * @brief Mesh structure.
 *
 * This structure is used to represent a mesh.
 * It contains the vertices and indices of the mesh.
 */
struct Mesh {
    std::vector<glm::vec3> vertices{};
    std::vector<glm::vec3> normals{};
    std::vector<glm::vec2> texCoords{};
    std::vector<uint32_t> indices{};

    explicit Mesh() = default;
    ~Mesh() = default;

    // Move constructor
    Mesh(Mesh &&other) = default;
    // Move assignment operator
    Mesh &operator=(Mesh &&other) = default;

    // Copy constructor
    Mesh(const Mesh &mesh) = default;
    // Copy assignment operator
    Mesh &operator=(const Mesh &other) = default;
};
} // namespace Plugin::Object::Component
