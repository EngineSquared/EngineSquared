#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utils/Viewer/Viewer.hpp"
namespace ES::Plugin::Window::Resource {
class Camera {
  public:
    Camera(int w, int h)
        : size(w, h), viewer(glm::vec3(5, 5, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), 45.0f, (float) w / h){};
    glm::vec2 size;
    float aspect;
    Utils::Viewer viewer;
    glm::mat4 view;
    glm::mat4 projection;
};
} // namespace ES::Plugin::Window::Resource
