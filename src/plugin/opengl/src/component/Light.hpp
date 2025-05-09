#pragma once

#include <glm/gtc/type_ptr.hpp>

namespace ES::Plugin::OpenGL::Component {

/**
 * @brief Structure to hold light information.
 *
 * This structure contains the type of the light and its colour.
 * The `type` field is an unsigned integer representing the type of light
 * (e.g., point, ambient, etc.).
 * The `colour` field is a 3D vector representing the RGB colour of the light.
 * The `intensity` field is a float representing the intensity of the light.
 */
struct Light {
    enum class TYPE : uint32_t {
        POINT = 0,
        AMBIENT = 1
    };

    TYPE type;
    glm::vec3 colour;
    float intensity;

    /**
     * @param   type      Type of the light. Default is POINT.
     * @param   colour    Colour of the light. Default is (1, 1, 1).
     * @param   intensity Intensity of the light. Default is 1.0.
     */
    Light(TYPE type = TYPE::POINT, glm::vec3 colour = glm::vec3(1), float intensity = 1.0f)
        : type(type), colour(colour), intensity(intensity)
    {
    }

    Light(const Light &) = default;
    Light(Light &&) = default;

    Light &operator=(const Light &) = default;
    Light &operator=(Light &&) = default;
};

} // namespace ES::Plugin::OpenGL::Component
