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
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-12-06
 **************************************************************************/

#ifndef MESH_HPP_
#define MESH_HPP_

#include "OBJLoader.hpp"

#include <entt/resource/loader.hpp>

namespace ES::Plugin::Object::Component {

/**
 * @brief Mesh structure.
 *
 * This structure is used to represent a mesh.
 * It contains the vertices and indices of the mesh.
 */
struct Mesh {
    std::vector<Component::Vertex> vertices;
    std::vector<uint32_t> indices;

    explicit Mesh(const std::string &file) { Resource::OBJLoader::loadModel(file, vertices, indices); }
};

/**
 * @brief MeshLoader structure.
 *
 * This structure is used to load a mesh from a file.
 * @note This structure is used by the entt resource cache.
 */
struct MeshLoader : entt::resource_loader<Mesh> {
    std::shared_ptr<Mesh> load(const std::string &file) const { return std::make_shared<Mesh>(file); }
};

} // namespace ES::Plugin::Object::Component

#endif /* !MESH_HPP_ */
