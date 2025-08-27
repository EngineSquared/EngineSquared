#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

// TODO: move
struct Shape {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<uint32_t> indices;
};

namespace ES::Plugin::Object::Utils {
void CombineShapesIntoMesh(const std::vector<Shape> &shapes, std::vector<glm::vec3> &outVertices,
                           std::vector<glm::vec3> &outNormals, std::vector<glm::vec2> &outTexCoords,
                           std::vector<uint32_t> &outIndices);
} // namespace ES::Plugin::Object::Utils
