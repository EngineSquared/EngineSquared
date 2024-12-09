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
 * @file Vertex.hpp
 * @brief Vertex struct declaration.
 *
 * This struct is used to represent a vertex in a mesh.
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-12-06
 **************************************************************************/

#ifndef VERTEX_HPP_
#define VERTEX_HPP_

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace ES::Plugin::Object::Component {

/**
 * @brief Vertex structure.
 *
 * This structure is used to represent a vertex in a mesh.
 * It contains the position, color, and texture coordinates of the vertex.
 */
struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    bool operator==(const Vertex &other) const = default;
};

} // namespace ES::Plugin::Object::Component

namespace std {

/**
 * @brief Hash function for the Vertex structure.
 */
template <> struct hash<ES::Plugin::Object::Component::Vertex> {
    /**
     * @brief Hash function for the Vertex structure.
     *
     * @param vertex Vertex to hash.
     * @return std::size_t Hash value.
     */
    size_t operator()(ES::Plugin::Object::Component::Vertex const &vertex) const
    {
        return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
               (hash<glm::vec2>()(vertex.texCoord) << 1);
    }
};

} // namespace std

#endif /* !VERTEX_HPP_ */
