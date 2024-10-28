#pragma once

namespace ES::Plugin::Colors::Utils {
struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

inline static const Color BLACK_COLOR = {0, 0, 0, 255};
inline static const Color WHITE_COLOR = {255, 255, 255, 255};
inline static const Color RED_COLOR = {255, 0, 0, 255};
inline static const Color GREEN_COLOR = {0, 255, 0, 255};
inline static const Color BLUE_COLOR = {0, 0, 255, 255};
inline static const Color YELLOW_COLOR = {255, 255, 0, 255};
inline static const Color MAGENTA_COLOR = {255, 0, 255, 255};
inline static const Color CYAN_COLOR = {0, 255, 255, 255};
inline static const Color GRAY_COLOR = {128, 128, 128, 255};
inline static const Color DARKGRAY_COLOR = {64, 64, 64, 255};

inline bool operator==(const Color &lhs, const Color &rhs)
{
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
}

inline bool operator!=(const Color &lhs, const Color &rhs) { return !(lhs == rhs); }

} // namespace ES::Plugin::Colors::Utils
