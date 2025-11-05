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
 * @file Shape.hpp
 * @brief Shape struct declaration.
 *
 * This struct is used to represent a shape.
 *
 * @author @EngineSquared
 * @version 0.1.1
 * @date 2025-10-04
 **************************************************************************/

#pragma once

#include "component/Material.hpp"
#include "component/Mesh.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Object::Resource {

/**
 * @brief Shape structure.
 *
 * This structure is used to represent a shape.
 * It contains the mesh and material of the shape.
 */
struct Shape {
    Component::Mesh mesh{};
    Component::Material material{};

    Shape() = default;
    ~Shape() = default;

    // Move constructor
    Shape(Shape &&other) = default;
    // Move assignment operator
    Shape &operator=(Shape &&other) = default;

    // Copy constructor
    Shape(const Shape &other) = default;
    // Copy assignment operator
    Shape &operator=(const Shape &other) = default;

    /**
     * @brief Get the Mesh object of the shape.
     *
     * @return Component::Mesh&  Reference to the mesh object.
     */
    Component::Mesh &GetMesh() { return mesh; }

    /**
     * @brief Get the Mesh object of the shape (const version).
     *
     * @return const Component::Mesh&  Const reference to the mesh object.
     */
    const Component::Mesh &GetMesh() const { return mesh; }

    /**
     * @brief Get the Material object of the shape.
     *
     * @return Component::Material&  Reference to the material object.
     */
    Component::Material &GetMaterial() { return material; }

    /**
     * @brief Get the Material object of the shape (const version).
     *
     * @return const Component::Material&  Const reference to the material object.
     */
    const Component::Material &GetMaterial() const { return material; }
};

} // namespace Object::Resource
