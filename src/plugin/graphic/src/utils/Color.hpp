#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace Graphic::Utils {
/**
 * @brief Color utility functions for converting RGB values to normalized vectors.
 *
 * Provides helper functions to simplify color handling by converting standard
 * 0-255 RGB values to normalized glm::vec3 values (0.0-1.0 range).
 */
class Color {
  public:
    /**
     * @brief Converts 8-bit RGB values (0-255) to normalized glm::vec3 (0.0-1.0).
     *
     * Eliminates the need for manual division by 255.0f for each channel,
     * reducing boilerplate and potential for errors.
     *
     * @param r Red channel (0-255)
     * @param g Green channel (0-255)
     * @param b Blue channel (0-255)
     * @return glm::vec3 Normalized color vector with values in range [0.0, 1.0]
     *
     * @example
     * auto color = Color::FromRGB(108, 193, 5);
     * // Returns glm::vec3(0.424f, 0.757f, 0.020f)
     */
    static inline glm::vec3 FromRGB(uint8_t r, uint8_t g, uint8_t b)
    {
        return glm::vec3(r / 255.f, g / 255.f, b / 255.f);
    }

    /**
     * @brief Converts a hex color code to normalized glm::vec3.
     *
     * Allows for convenient use of hex color codes commonly found in
     * design tools and color palettes.
     *
     * @param hex Color as 24-bit hex value (0xRRGGBB)
     * @return glm::vec3 Normalized color vector with values in range [0.0, 1.0]
     *
     * @example
     * auto color = Color::FromHex(0x6CC1FF);
     * // Extracts RGB(108, 193, 255) and returns normalized vector
     */
    static inline glm::vec3 FromHex(uint32_t hex)
    {
        uint8_t r = (hex >> 16) & 0xFF;
        uint8_t g = (hex >> 8) & 0xFF;
        uint8_t b = hex & 0xFF;
        return FromRGB(r, g, b);
    }
};
} // namespace Graphic::Utils
