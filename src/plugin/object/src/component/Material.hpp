/**************************************************************************
 * EngineSquared v0.1.1
 *
 * EngineSquared is a software package, part of the Engine².
 *
 * This file is part of the EngineSquared project that is under MIT License.
 * Copyright © 2025-present by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the MIT License. See the project's LICENSE file for
 * the full license text and details.
 *
 * @file Material.hpp
 * @brief Material struct declaration.
 *
 * This struct is used to represent a material.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-10-04
 **************************************************************************/

#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Plugin::Object::Component {

/**
 * @brief Material structure.
 *
 * This structure is used to represent a material.
 * It contains the properties of the material such as color, shininess, and texture.
 * It is designed to be compatible with the TinyObjLoader material representation.
 * See:
 * - https://github.com/tinyobjloader/tinyobjloader/blob/release/tiny_obj_loader.h (look for `material_t` struct)
 */
struct Material {
    std::string name;

    glm::dvec3 ambient;
    glm::dvec3 diffuse;
    glm::dvec3 specular;
    glm::dvec3 transmittance;
    glm::dvec3 emission;
    double shininess;
    double ior;       // index of refraction
    double dissolve;  // 1 == opaque; 0 == fully transparent

    explicit Material() = default;
    ~Material() = default;

    // Move constructor
    Material(Material &&other) = default;
    // Move assignment operator
    Material &operator=(Material &&other) = default;

    // Copy constructor
    explicit Material(const Material &mesh) = default;
    // Copy assignment operator
    Material &operator=(const Material &other) = default;
};

} // namespace Plugin::Object::Component
