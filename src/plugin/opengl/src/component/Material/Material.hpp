#pragma once

#include <string>

namespace ES::Plugin::OpenGL::Component
{
    /**
     * Material component
     * This is only a reference to the material used by the entity,
     * handling of the material is done via the MaterialCache resource.
     */
    struct Material
    {
        std::string name;
    };
} // namespace ES::Plugin::OpenGL::Component