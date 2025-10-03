/**************************************************************************
 * EngineSquared v0.1.1
 *
 * EngineSquared is a software package, part of the Engine².
 *
 * This file is part of the EngineSquared project and is licensed under
 * the MIT License. See the project's LICENSE file for the full license
 * text and details.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file OBJLoader.hpp
 * @brief OBJLoader class declaration.
 *
 * This class is used to load and parse OBJ files.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-10-03
 **************************************************************************/

#pragma once

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <string>

#include "component/Mesh.hpp"
#include "exception/OBJLoaderError.hpp"

namespace Plugin::Object {

/**
 * @brief OBJLoader is a class that loads and parses OBJ files.
 *
 * It utilizes the TinyObjLoader library to read the contents of an OBJ file and
 * extract mesh data such as vertices, normals, texture coordinates, and indices.
 * The loaded data is stored in a Component::Mesh object, which can be retrieved
 * using the GetMesh() method.
 *
 * @example "Loading an OBJ file"
 * @code
 * try {
 *     Plugin::Object::OBJLoader loader("path/to/model.obj");
 *     Plugin::Object::Component::Mesh mesh = loader.GetMesh();
 * } catch (const Plugin::Object::OBJLoaderError &e) {
 *     std::cerr << e.what() << std::endl;
 * }
 * @endcode
 *
 * @see Component::Mesh
 * @see OBJLoaderError
 */
class OBJLoader {
  public:
    /**
     * @brief Constructs an OBJLoader for the specified file.
     *
     * @param filepath The path to the OBJ file.
     * @param mtlSearchPath The path to the directory containing material files.
     *
     * @throws OBJLoaderError if the file cannot be loaded or parsed.
     * @see OBJLoaderError
     */
    explicit OBJLoader(const std::string &filepath, const std::string &mtlSearchPath = "");
    ~OBJLoader() = default;

    /**
     * @brief Retrieves the loaded mesh data.
     *
     * @return Component::Mesh The mesh data extracted from the OBJ file.
     *
     * @see Component::Mesh
     */
    [[nodiscard]] Component::Mesh GetMesh();

  protected:
  private:
    tinyobj::ObjReaderConfig _reader_config;
    tinyobj::ObjReader _reader;
};

} // namespace Plugin::Object
