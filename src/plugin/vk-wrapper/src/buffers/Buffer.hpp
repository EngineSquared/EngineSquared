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
 * @file Buffer.hpp
 * @brief Buffer class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-11-03
 **************************************************************************/

#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include "ImageView.hpp"
#include "Texture.hpp"
#include "UniformObject.hpp"
#include "Vertex.hpp"

#include <chrono>
#include <entt/core/hashed_string.hpp>
#include <entt/resource/cache.hpp>
#include <unordered_map>

namespace ES::Plugin::Wrapper {

const uint32_t MAX_FRAMES_IN_FLIGHT = 2;

/**
 * @brief Buffers class.
 *
 * This class is used to represent the buffers in the Vulkan API.
 * It contains the vertex buffer, the index buffer, and the uniform buffer.
 * The buffers are used to store the vertex data, index data, and uniform data.
 *
 * @example
 * @code
 * Buffers buffers;
 * Buffers::CreateInfo info = {
 *   .device = device,
 *   .physicalDevice = physicalDevice,
 *   .commandPool = commandPool,
 *   .graphicsQueue = graphicsQueue,
 *   .swapChainImages = swapChainImages,
 * };
 * buffers.Create(info, textures);
 * buffers.Destroy(device, swapChainImages);
 * @endcode
 */
class Buffers {
  public:
    /**
     * @brief Structure to hold the creation information for the Buffers.
     *
     * This structure contains all the necessary information required to create the Buffers,
     * including the device, the physical device, the command pool, the graphics queue, the swap chain images,
     * and the textures.
     *
     * @param device The Vulkan device.
     * @param physicalDevice The Vulkan physical device.
     * @param commandPool The Vulkan command pool.
     * @param graphicsQueue The Vulkan graphics queue.
     * @param swapChainImages The swap chain images. Only used for the uniform buffer.
     */
    struct CreateInfo {
        VkDevice device;
        VkPhysicalDevice physicalDevice;
        VkCommandPool commandPool;
        VkQueue graphicsQueue;
        std::vector<VkImage> swapChainImages;
    };

  public:
    /**
     * @brief Create the VertexBuffer object, the IndexBuffer object and the UniformBuffer object.
     *
     * @param info The creation information required for the Buffers.
     * @param textures The textures.
     */
    void Create(const CreateInfo &info, const entt::resource_cache<Texture, TextureLoader> &textures);

    /**
     * @brief Destroy the VertexBuffer object, the IndexBuffer object and the UniformBuffer object.
     *
     * @note The uniform buffer is destroyed in the DestroyUniformBuffers function.
     *
     * @param device The Vulkan device.
     * @param swapChainImages The swap chain images.
     */
    void Destroy(const VkDevice &device, const std::vector<VkImage> &swapChainImages);

    /**
     * @brief Update the Uniform Buffer object in the Vulkan API.
     *
     * @param device  The Vulkan device.
     * @param swapChainExtent  The swap chain extent.
     * @param currentImage  The current image.
     */
    void UpdateUniformBuffer(const VkDevice &device, const VkExtent2D swapChainExtent, const uint32_t currentImage);

    /**
     * @brief Get the uniform buffers.
     *
     * @return const std::vector<VkBuffer>& The uniform buffers.
     */
    [[nodiscard]] const std::vector<VkBuffer> &GetUniformBuffers() const { return _uniformBuffers; }

    /**
     * @brief Get the vertex buffer.
     *
     * @return const VkBuffer& The vertex buffer.
     */
    [[nodiscard]] const VkBuffer &GetVertexBuffer() const { return _vertexBuffer; }

    /**
     * @brief Get the index buffer.
     *
     * @return const VkBuffer& The index buffer.
     */
    [[nodiscard]] const VkBuffer &GetIndexBuffer() const { return _indexBuffer; }

  private:
    /**
     * @brief Create a Vertex Buffer object in the Vulkan API.
     *
     * @param device  The Vulkan device.
     * @param physicalDevice  The Vulkan physical device.
     * @param commandPool  The Vulkan command pool.
     * @param graphicsQueue  The Vulkan graphics queue.
     */
    void CreateVertexBuffer(const VkDevice &device, const VkPhysicalDevice &physicalDevice,
                            const VkCommandPool &commandPool, const VkQueue &graphicsQueue);

    /**
     * @brief Create a Index Buffer object in the Vulkan API.
     *
     * @param device  The Vulkan device.
     * @param physicalDevice  The Vulkan physical device.
     * @param commandPool  The Vulkan command pool.
     * @param graphicsQueue  The Vulkan graphics queue.
     */
    void CreateIndexBuffer(const VkDevice &device, const VkPhysicalDevice &physicalDevice,
                           const VkCommandPool &commandPool, const VkQueue &graphicsQueue);

    /**
     * @brief Create a Uniform Buffer object in the Vulkan API.
     *
     * Updates the buffer by applying a transformation to its contents at each frame. The buffer must be destroyed at
     * the end of the program. Depends on the number of frames in the swap chain.
     *
     * @param device  The Vulkan device.
     * @param physicalDevice  The Vulkan physical device.
     * @param swapChainImages  The swap chain images.
     */
    void CreateUniformBuffer(const VkDevice &device, const VkPhysicalDevice &physicalDevice,
                             const std::vector<VkImage> &swapChainImages);

    /**
     * @brief Create a Texture Buffer object in the Vulkan API.
     *
     * The texture buffer is used to store the texture data from an image file.
     *
     * @param device  The Vulkan device.
     * @param physicalDevice  The Vulkan physical device.
     * @param commandPool  The Vulkan command pool.
     * @param graphicsQueue  The Vulkan graphics queue.
     * @param texture  The texture.
     */
    void CreateTextureBuffer(const VkDevice &device, const VkPhysicalDevice &physicalDevice,
                             const VkCommandPool &commandPool, const VkQueue &graphicsQueue, Texture &texture);

    /**
     * @brief Create a Texture View object in the Vulkan API.
     *
     * @param device  The Vulkan device.
     * @param texture  The texture.
     */
    void CreateTextureView(const VkDevice &device, Texture &texture);

    /**
     * @brief Create a Texture Sampler object in the Vulkan API.
     *
     * @param device  The Vulkan device.
     * @param physicalDevice  The Vulkan physical device.
     * @param texture  The texture.
     */
    void CreateTextureSampler(const VkDevice &device, const VkPhysicalDevice &physicalDevice, Texture &texture);

    /**
     * @brief Create a Buffer object in the Vulkan API.
     *
     * @param device  The Vulkan device.
     * @param physicalDevice  The Vulkan physical device.
     * @param size  The size of the buffer.
     * @param usage  The usage of the buffer.
     * @param properties  The properties of the buffer.
     * @param buffer  The buffer.
     * @param bufferMemory  The buffer memory.
     */
    void CreateBuffer(const VkDevice &device, const VkPhysicalDevice &physicalDevice, const VkDeviceSize size,
                      const VkBufferUsageFlags usage, const VkMemoryPropertyFlags properties, VkBuffer &buffer,
                      VkDeviceMemory &bufferMemory);

    void CreateImage(const VkDevice &device, const VkPhysicalDevice &physicalDevice, VkFormat format,
                     VkImageTiling tiling, VkImageUsageFlags usage, Texture &texture);

    /**
     * @brief Find the memory type in the physical device.
     *
     * @details The memory type is found by the type filter and the properties.
     *          The memmory type is used to allocate the buffer and image memory.
     *
     * @param physicalDevice The Vulkan physical device.
     * @param typeFilter The type filter.
     * @param properties The properties.
     * @return uint32_t The memory type index.
     */
    uint32_t FindMemoryType(const VkPhysicalDevice &physicalDevice, const uint32_t typeFilter,
                            const VkMemoryPropertyFlags properties);

    /**
     * @brief Copy a buffer from the source buffer to the destination buffer.
     *
     * @param device  The Vulkan device.
     * @param commandPool  The Vulkan command pool.
     * @param graphicsQueue  The Vulkan graphics queue.
     * @param srcBuffer  The source buffer.
     * @param dstBuffer  The destination buffer.
     * @param size  The size of the buffer.
     */
    void CopyBuffer(const VkDevice &device, const VkCommandPool &commandPool, const VkQueue &graphicsQueue,
                    const VkBuffer &srcBuffer, const VkBuffer &dstBuffer, VkDeviceSize size);

    /**
     * @brief Transition the image layout in the Vulkan API.
     *
     * @param device  The Vulkan device.
     * @param commandPool  The Vulkan command pool.
     * @param graphicsQueue  The Vulkan graphics queue.
     * @param image  The image.
     * @param format  The format of the image.
     * @param oldLayout  The old layout of the image.
     * @param newLayout  The new layout of the image.
     */
    void TransitionImageLayout(const VkDevice &device, const VkCommandPool &commandPool, const VkQueue &graphicsQueue,
                               const VkImage &image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

    /**
     * @brief Copy a buffer to an image in the Vulkan API.
     *
     * @param device  The Vulkan device.
     * @param commandPool  The Vulkan command pool.
     * @param graphicsQueue  The Vulkan graphics queue.
     * @param buffer  The buffer.
     * @param texture  The texture.
     */
    void CopyBufferToImage(const VkDevice &device, const VkCommandPool &commandPool, const VkQueue &graphicsQueue,
                           VkBuffer buffer, Texture &texture);

    /**
     * @brief Begin a single time command in the Vulkan API.
     *
     * @param device  The Vulkan device.
     * @param commandPool  The Vulkan command pool.
     * @return VkCommandBuffer  The command buffer.
     */
    VkCommandBuffer BeginSingleTimeCommands(const VkDevice &device, const VkCommandPool &commandPool);

    /**
     * @brief End a single time command in the Vulkan API.
     *
     * @param device  The Vulkan device.
     * @param commandPool  The Vulkan command pool.
     * @param graphicsQueue  The Vulkan graphics queue.
     * @param commandBuffer  The command buffer.
     */
    void EndSingleTimeCommands(const VkDevice &device, const VkCommandPool &commandPool, const VkQueue &graphicsQueue,
                               VkCommandBuffer commandBuffer);

  private:
    VkBuffer _vertexBuffer;
    VkDeviceMemory _vertexBufferMemory;
    VkBuffer _indexBuffer;
    VkDeviceMemory _indexBufferMemory;
    std::vector<VkBuffer> _uniformBuffers;
    std::vector<VkDeviceMemory> _uniformBuffersMemory;
    std::vector<void *> _uniformBuffersMapped;
    VkImageView _textureView;
};

} // namespace ES::Plugin::Wrapper

#endif /* !BUFFER_HPP_ */
