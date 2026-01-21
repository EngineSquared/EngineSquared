/**************************************************************************
 * EngineSquared v0.2.0
 *
 * EngineSquared is a software package, part of the Engine² organization.
 *
 * This file is part of the EngineSquared project that is under MIT License.
 * Copyright © 2024-present by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the MIT License. See the project's LICENSE file for
 * the full license text and details.
 *
 * @file MeshUtils.hpp
 * @brief Utility functions for mesh manipulation.
 *
 * @author @EngineSquared
 * @version 0.2.0
 * @date 2026-01-15
 **************************************************************************/

#pragma once

#include "component/Mesh.hpp"

namespace Object::Utils {

/**
 * @brief Recalculate normals for a mesh based on face geometry.
 *
 * This function computes smooth normals by averaging the face normals
 * of all triangles that share each vertex. This is essential for correct
 * lighting on deformable meshes like soft bodies.
 *
 * Algorithm:
 * 1. Initialize all normals to zero
 * 2. For each triangle face, compute face normal using cross product
 * 3. Add face normal to each vertex's accumulated normal
 * 4. Normalize all vertex normals
 *
 * @param mesh The mesh to recalculate normals for. Must have:
 *             - Non-empty vertices array
 *             - Non-empty indices array (multiple of 3 for triangles)
 *             - Normals array same size as vertices (will be overwritten)
 *
 * @note The mesh's dirty flag is NOT modified by this function.
 *       The caller is responsible for managing the dirty state.
 *
 * @note For "flat" meshes where each face has unique vertices (like OBJ imports),
 *       this will still work correctly as each vertex only belongs to one face.
 */
void RecalculateNormals(Component::Mesh &mesh);

/**
 * @brief Validate mesh data integrity.
 *
 * Checks that vertices, normals, and texCoords arrays have matching sizes.
 * This is a debug utility to catch malformed meshes early.
 *
 * @param mesh The mesh to validate.
 * @return true if mesh arrays have consistent sizes.
 * @return false if there's a size mismatch.
 */
[[nodiscard]] bool ValidateMeshArraySizes(const Component::Mesh &mesh);

} // namespace Object::Utils
