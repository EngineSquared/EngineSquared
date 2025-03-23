#pragma once

#include <GL/glew.h>
#include <glm/vec3.hpp>

namespace ES::Plugin::OpenGL::Utils {
class MaterialData {
  public:
    MaterialData() = default;
    ~MaterialData() = default;

    MaterialData(const MaterialData &) = default;
    MaterialData(MaterialData &&) = default;

    MaterialData &operator=(const MaterialData &) = default;
    MaterialData &operator=(MaterialData &&) = default;

    GLfloat Shiness = 180;
    glm::vec3 Ka = glm::vec3(0.2, 0.2, 0.2);
    glm::vec3 Kd = glm::vec3(1, 0.3, 0.1);
    glm::vec3 Ks = glm::vec3(0.4, 0.4, 0.4);
};
} // namespace ES::Plugin::OpenGL::Utils
