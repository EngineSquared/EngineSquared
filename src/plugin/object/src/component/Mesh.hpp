/**************************************************************************
 * EngineSquared v0.0.0
 *
 * EngineSquared is a software package, part of the Engine².
 *
 * This file is part of the EngineSquared project that is under GPL-3.0 License.
 * Copyright © 2024 by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file Mesh.hpp
 * @brief Mesh struct declaration.
 *
 * This struct is used to represent a mesh.
 *
 * @author @EngineSquared
 * @version 0.0.0
 * @date 2024-12-06
 **************************************************************************/

#pragma once

#include <vector>
#include <glm/glm.hpp>

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
    explicit Mesh(const Mesh &mesh) = default;
    // Copy assignment operator
    Mesh &operator=(const Mesh &other) = default;
};
} // namespace Plugin::Object::Component
