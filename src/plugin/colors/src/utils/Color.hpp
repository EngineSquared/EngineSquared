#pragma once

namespace ES::Plugin::Colors::Utils {
/**
 * @brief A color with red, green, blue and alpha channels
 */
struct Color {
    /**
     * @brief Check if two colors are equal
     *
     * @param   lhs The first color
     * @param   rhs The second color
     */
    friend bool operator==(const Color &lhs, const Color &rhs) = default;

    // @brief Red channel (0.0 - 1.0)
    float red = 1.f;
    // @brief Green channel (0.0 - 1.0)
    float green = 1.f;
    // @brief Blue channel (0.0 - 1.0)
    float blue = 1.f;
    // @brief Alpha channel (0.0 - 1.0)
    float alpha = 1.f;
};

inline static const Color BLACK_COLOR = {0.f, 0.f, 0.f, 1.f};
inline static const Color WHITE_COLOR = {1.f, 1.f, 1.f, 1.f};
inline static const Color RED_COLOR = {1.f, 0.f, 0.f, 1.f};
inline static const Color GREEN_COLOR = {0.f, 1.f, 0.f, 1.f};
inline static const Color BLUE_COLOR = {0.f, 0.f, 1.f, 1.f};
inline static const Color YELLOW_COLOR = {1.f, 1.f, 0.f, 1.f};
inline static const Color MAGENTA_COLOR = {1.f, 0.f, 1.f, 1.f};
inline static const Color CYAN_COLOR = {0.f, 1.f, 1.f, 1.f};
inline static const Color LIGHTGRAY_COLOR = {0.75f, 0.75f, 0.75f, 1.f};
inline static const Color GRAY_COLOR = {0.5f, 0.5f, 0.5f, 1.f};
inline static const Color DARKGRAY_COLOR = {0.25f, 0.25f, 0.25f, 1.f};
} // namespace ES::Plugin::Colors::Utils
