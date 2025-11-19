#pragma once

#include "Sampler.hpp"
#include "resource/ResourceManager.hpp"

namespace Graphic::Resource {

/**
 * @brief SamplerContainer is a resource that stores Sampler resources.
 *
 * This class provides a convenient way to manage multiple Samplers in the graphics system.
 * It uses the ResourceManager template to store and retrieve Samplers by their unique identifiers.
 *
 * @see Object::Resource::ResourceManager
 * @see Graphic::Resource::Sampler
 */
using SamplerContainer = Object::Resource::ResourceManager<Sampler>;

} // namespace Graphic::Resource
