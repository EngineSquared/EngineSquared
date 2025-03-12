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
 * @file Descriptor.hpp
 * @brief Descriptor class declaration.
 *
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-11-04
 **************************************************************************/

#ifndef DESCRIPTOR_HPP_
#define DESCRIPTOR_HPP_

#include "Buffer.hpp"

namespace ES::Plugin::Wrapper {

/**
 * @brief Descriptor class.
 *
 *
 * @example
 * @code
 * Descriptor descriptor;
 * descriptor.Create(device);
 * descriptor.Destroy(device);
 * @endcode
 */
class Descriptor {
  public:
    /**
     * @brief Creates the descriptor set layout.
     *
     * This function creates the descriptor set layout.
     * The descriptor set layout is used to define the layout of the descriptor set.
     * It is linked to rendering pipelines operations. It defines the type of resources
     * that are going to be used in the shaders.
     *
     * @param device  The Vulkan device.
     */
    void Create(const VkDevice &device);

    /**
     * @brief Creates the descriptor pool.
     *
     * This function creates the descriptor pool.
     * The descriptor pool is used to allocate descriptor sets.
     *
     * @param device  The Vulkan device.
     */
    void CreateDescriptorPool(const VkDevice &device);

    /**
     * @brief Creates the descriptor set.
     *
     * This function creates the descriptor set.
     * The descriptor set is used to store the resources that are going to be used in the shaders.
     *
     * @param device  The Vulkan device.
     * @param uniformBuffer  The uniform buffer.
     * @param texture  The texture.
     */
    void CreateDescriptorSet(const VkDevice &device, const std::vector<VkBuffer> &uniformBuffer, Texture &texture);

    /**
     * @brief Destroys the descriptor set layout.
     *
     * This function destroys the descriptor set layout.
     *
     * @param device  The Vulkan device.
     */
    void Destroy(const VkDevice &device);

    /**
     * @brief Gets the descriptor set layout.
     *
     * This function returns the descriptor set layout.
     *
     * @return The descriptor set layout.
     */
    [[nodiscard]] VkDescriptorSetLayout Get() const { return _descriptorSetLayout; }

    /**
     * @brief Gets the descriptor pool.
     *
     * This function returns the descriptor pool.
     *
     * @return The descriptor pool.
     */
    [[nodiscard]] VkDescriptorPool GetDescriptorPool() const { return _descriptorPool; }

    /**
     * @brief Gets the descriptor sets.
     *
     * This function returns the descriptor sets.
     *
     * @return The descriptor sets.
     */
    [[nodiscard]] const std::vector<VkDescriptorSet> &GetDescriptorSets() const { return _descriptorSets; }

  private:
    VkDescriptorSetLayout _descriptorSetLayout;
    VkDescriptorPool _descriptorPool;
    std::vector<VkDescriptorSet> _descriptorSets;
};

} // namespace ES::Plugin::Wrapper

#endif /* !DESCRIPTOR_HPP_ */
