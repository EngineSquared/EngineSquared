#pragma once

#include <glm/glm.hpp>
#include <string>

#include "Audio.hpp"

namespace ES::Plugin::Sound::Component
{
struct AudioSource {
    ES::Plugin::Sound::Utils::Audio &sound;
    glm::vec3 position;
};
} // ES::Plugin::Sound::Component
