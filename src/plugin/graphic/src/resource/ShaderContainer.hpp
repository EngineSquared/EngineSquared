#pragma once

#include "resource/ResourceManager.hpp"
#include "resource/Shader.hpp"

namespace Graphic::Resource {

/**
 * @brief ShaderContainer is a resource that stores Shader resources.
 *
 * This class provides a convenient way to manage multiple shaders in the graphics system.
 * It uses the ResourceManager template to store and retrieve shaders by their unique identifiers.
 *
 * @see Object::Resource::ResourceManager
 * @see Graphic::Resource::Shader
 */
using ShaderContainer = Object::Resource::ResourceManager<Shader>;

} // namespace Graphic::Resource
