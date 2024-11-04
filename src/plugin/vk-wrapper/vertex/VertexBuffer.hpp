/**************************************************************************
 * VkWrapper v0.0.0
 *
 * VkWrapper is a software package, part of the Engine².
 *
 * This file is part of the VkWrapper project that is under GPL-3.0 License.
 * Copyright © 2024 by @EngineSquared, All rights reserved.
 *
 * VkWrapper is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file VertexBuffer.hpp
 * @brief VertexBuffer class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-11-03
 **************************************************************************/

#ifndef VERTEXBUFFER_HPP_
#define VERTEXBUFFER_HPP_

#include "Vertex.hpp"

namespace ES::Plugin::Wrapper {

/**
 * @brief VertexBuffer class.
 *
 * This class is used to represent a vertex buffer in the Vulkan API.
 * It contains the buffer and buffer memory.
 *
 * @example
 * @code
 * VertexBuffer vertexBuffer;
 * vertexBuffer.Create(device, physicalDevice);
 * vertexBuffer.Destroy(device);
 * @endcode
 */
class VertexBuffer {
  public:
    /**
     * @brief Create the VertexBuffer object.
     *
     * @param device The Vulkan device.
     * @param physicalDevice The Vulkan physical device.
     */
    void Create(const VkDevice &device, const VkPhysicalDevice &physicalDevice);

    /**
     * @brief Destroy the VertexBuffer object.
     *
     * @param device The Vulkan device.
     */
    void Destroy(const VkDevice &device);

    /**
     * @brief Get the buffer.
     *
     * @return const VkBuffer& The buffer.
     */
    [[nodiscard]] const VkBuffer &Get() const { return _buffer; }

  private:
    /**
     * @brief Find the memory type in the physical device.
     *
     * @param physicalDevice The Vulkan physical device.
     * @param typeFilter The type filter.
     * @param properties The properties.
     * @return uint32_t The memory type index.
     */
    uint32_t FindMemoryType(const VkPhysicalDevice &physicalDevice, const uint32_t typeFilter,
                            const VkMemoryPropertyFlags properties);

  private:
    VkBuffer _buffer;
    VkDeviceMemory _bufferMemory;
};

} // namespace ES::Plugin::Wrapper

#endif /* !VERTEXBUFFER_HPP_ */
