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

#include <entt/core/hashed_string.hpp>
#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace Object::Component {

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
    using Id = entt::hashed_string;

    std::string name;

    glm::vec3 ambient = glm::vec3(1.f, 1.f, 1.f);
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 transmittance;
    glm::vec3 emission;
    float shininess;
    float ior;      // index of refraction
    float dissolve; // 1 == opaque; 0 == fully transparent

    std::string diffuseTexName;

    explicit Material() = default;
    ~Material() = default;

    // Copy
    explicit Material(const Material &other) = default;
    Material &operator=(const Material &other) = default;

    // Move
    Material(Material &&other) = default;
    Material &operator=(Material &&other) = default;
};

} // namespace Object::Component
