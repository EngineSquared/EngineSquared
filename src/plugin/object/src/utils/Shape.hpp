#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

struct Shape {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texCoords;
    std::vector<uint32_t> indices;
};
