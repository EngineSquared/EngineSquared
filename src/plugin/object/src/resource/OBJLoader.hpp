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
#include <string>
#include <tiny_obj_loader.h>

#include "component/Material.hpp"
#include "component/Mesh.hpp"
#include "exception/OBJLoaderError.hpp"
#include "resource/Shape.hpp"

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
 * @example "Getting shapes from an OBJ file"
 * @code
 * for (const auto &shape : loader.GetShapes()) {
 *     const auto &mesh = shape.GetMesh();
 * }
 * @endcode
 *
 * @example "Getting materials from an OBJ file"
 * @code
 * for (const auto &material : loader.GetMaterials()) {
 *     std::cout << "Material: " << material.name << std::endl;
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

    /**
     * @brief Retrieves the loaded shapes data.
     *
     * @return std::vector<Resource::Shape> The shapes data extracted from the OBJ file.
     *
     * @see Resource::Shape
     */
    [[nodiscard]] std::vector<Resource::Shape> GetShapes();

    /**
     * @brief Retrieves the loaded materials data.
     *
     * @return std::vector<Component::Material> The materials data extracted from the OBJ file.
     *
     * @see Component::Material
     */
    [[nodiscard]] std::vector<Component::Material> GetMaterials();

  private:
    /**
     * @brief Processes a single face of the mesh and populates the Mesh object.
     *
     * @param mesh The Mesh object to populate.
     * @param shapes The vector of shapes loaded from the OBJ file.
     * @param shape The index of the current shape being processed.
     * @param face_vertices The number of vertices in the current face.
     * @param index_offset The offset in the index array for the current face.
     *
     * @see Component::Mesh
     */
    void ProcessMeshFace(Component::Mesh &mesh, const std::vector<tinyobj::shape_t> &shapes, size_t shape,
                         size_t face_vertices, size_t &index_offset) noexcept;

    /**
     * @brief Sets the properties of a material according to the tinyobj::material_t structure.
     *
     * @param material  The Material object to populate.
     * @param mat  The tinyobj::material_t object containing the material properties.
     */
    void SetMaterialProperties(Component::Material &material, const tinyobj::material_t &mat) noexcept;

  protected:
  private:
    tinyobj::ObjReaderConfig _reader_config;
    tinyobj::ObjReader _reader;
    Component::Mesh _mesh{};
    std::vector<Resource::Shape> _shapes{};
    std::vector<Component::Material> _materials{};
};

} // namespace Plugin::Object
