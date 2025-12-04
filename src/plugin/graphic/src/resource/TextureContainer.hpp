#pragma once

#include "resource/ResourceManager.hpp"
#include "resource/Texture.hpp"

namespace Graphic::Resource {

/**
 * @brief TextureContainer is a resource that stores Texture resources.
 *
 * This class provides a convenient way to manage multiple textures in the graphics system.
 * It uses the ResourceManager template to store and retrieve textures by their unique identifiers.
 *
 * @see Object::Resource::ResourceManager
 * @see Graphic::Resource::Texture
 */
using TextureContainer = Object::Resource::ResourceManager<Texture>;

} // namespace Graphic::Resource
