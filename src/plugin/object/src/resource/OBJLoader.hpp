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
 * @file OBJLoader.hpp
 * @brief OBJLoader class declaration.
 *
 * This class is used to load .obj files and create a model from them.
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-12-06
 **************************************************************************/

#ifndef OBJLOADER_HPP_
#define OBJLOADER_HPP_

#include <memory>
#include <string>
#include <vector>

#include "component/Vertex.hpp"

#if defined(VULKAN)
#    define VK_REVERSE_Y_TEX 1.0f - // NOSONAR
#else
#    define VK_REVERSE_Y_TEX
#endif

namespace ES::Plugin::Object::Resource {

/**
 * @brief OBJLoader class.
 *
 * This class is used to load .obj files.
 */
class OBJLoader {
  public:
    /**
     * Load a model from a .obj file.
     *
     * @param path path to the .obj file
     * @param vertices vector to store the vertices
     * @param normals vector to store the normals
     * @param texCoords vector to store the texture coordinates
     * @param indices vector to store the indices
     * @return true if the model was loaded successfully, false otherwise
     */
    static bool loadModel(const std::string &path, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals,
                          std::vector<glm::vec2> &texCoords, std::vector<uint32_t> &indices);
};

} // namespace ES::Plugin::Object::Resource

#endif /* !OBJLOADER_HPP_ */
