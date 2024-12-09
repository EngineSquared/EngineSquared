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
#include <stdexcept>
#include <string>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "Vertex.hpp"

#include "Logger.hpp"

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
     * @param indices vector to store the indices
     * @return true if the model was loaded successfully, false otherwise
     */
    static bool loadModel(const std::string &path, std::vector<Component::Vertex> &vertices,
                          std::vector<uint32_t> &indices);
};

} // namespace ES::Plugin::Object::Resource

#endif /* !OBJLOADER_HPP_ */
