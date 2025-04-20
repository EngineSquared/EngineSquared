#pragma once

#include <glm/gtc/type_ptr.hpp>

namespace ES::Plugin::OpenGL::Component {

/**
 * @brief Structure to hold light information.
 *
 * This structure contains the type of the light and its intensity.
 * The `type` field is an unsigned integer representing the type of light
 * (e.g., point, ambient, etc.).
 * The `intensity` field is a 3D vector representing the RGB intensity
 * of the light.
 */
struct Light {
    enum class TYPE : uint32_t {
        POINT = 0,
        AMBIENT = 1
    };

    TYPE type;
    glm::vec3 intensity;

    /**
     * @param   type      Type of the light. Default is POINT.
     * @param   intensity Intensity of the light. Default is (1, 1, 1).
     */
    Light(TYPE type = TYPE::POINT, glm::vec3 intensity = glm::vec3(1)) : type(type), intensity(intensity) {}

    Light(const Light &) = default;
    Light(Light &&) = default;

    Light &operator=(const Light &) = default;
    Light &operator=(Light &&) = default;
};

} // namespace ES::Plugin::OpenGL::Component
