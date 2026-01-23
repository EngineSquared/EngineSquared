/*
** EPITECH PROJECT, 2026
** EngineSquared
** File description:
** OnResize
*/

#pragma once

#include <glm/vec2.hpp>

namespace Window::Event {

struct OnResize {
    glm::uvec2 newSize;
};
} // namespace Window::Event
