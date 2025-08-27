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
 * @file MeshFaces.hpp
 * @brief MeshFaces struct declaration.
 *
 * This struct is used to represent mesh faces with individual materials.
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-12-06
 **************************************************************************/

#pragma once

#include <vector>
#include <cstdint>

#include <entt/entt.hpp>

namespace ES::Plugin::Object::Component {

/**
 * @brief Face structure.
 *
 * This structure represents a single face with its indices and material.
 */
struct Face {
    std::vector<uint32_t> indices{};
    entt::hashed_string materialId{};

    explicit Face() = default;
    explicit Face(const std::vector<uint32_t> &indices, const entt::hashed_string &materialId)
        : indices(indices), materialId(materialId) {}
    explicit Face(const Face &face) = default;
    ~Face() = default;

    // Move constructor
    Face(Face &&other) = default;
};

/**
 * @brief MeshFaces structure.
 *
 * This structure is used to represent a mesh with individual face materials.
 * It contains a collection of faces, each with their own material.
 */
struct MeshFaces {
    std::vector<Face> faces{};

    explicit MeshFaces() = default;
    explicit MeshFaces(const std::vector<Face> &faces) : faces(faces) {}
    explicit MeshFaces(const MeshFaces &meshFaces) = default;
    ~MeshFaces() = default;

    // Move constructor
    MeshFaces(MeshFaces &&other) = default;
};

} // namespace ES::Plugin::Object::Component
