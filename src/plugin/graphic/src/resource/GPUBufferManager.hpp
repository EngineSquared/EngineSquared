#pragma once

#include "resource/ResourceManager.hpp"
#include "resource/AGPUBuffer.hpp"

namespace Graphic::Resource {

/**
 * @brief GPUBufferContainer is a resource that stores GPUBuffer resources.
 *
 * This class provides a convenient way to manage multiple GPUBuffers in the graphics system.
 * It uses the ResourceManager template to store and retrieve GPUBuffers by their unique identifiers.
 *
 * @see Object::Resource::ResourceManager
 * @see Graphic::Resource::GPUBuffer
 */
using GPUBufferContainer = Object::Resource::ResourceManager<std::unique_ptr<AGPUBuffer>>;

} // namespace Graphic::Resource
