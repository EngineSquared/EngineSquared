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
 * @file VkWrapper.hpp
 * @brief VkWrapper class declaration.
 *
 * This class is a wrapper for the Vulkan API.
 * It is used to simplify the use of Vulkan in the Engine².
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-15
 **************************************************************************/

#ifndef VKWRAPPER_HPP_
#define VKWRAPPER_HPP_

#include "Instance.hpp"

namespace ES::Plugin {

/**
 * @brief VkWrapper class.
 *
 * This class is a wrapper for the Vulkan API.
 * It is used to simplify the use of Vulkan in the Engine².
 *
 * @example "Usage of the VkWrapper class:"
 * @code
 * ES::Plugin::Window::Resource::Window window(800, 600, "My Engine");
 *
 * ES::Plugin::VkWrapper vkWrapper;
 * ES::Plugin::VkWrapper::CreateInfo createInfo;
 *
 * vkWrapper.CreateInstance(window.GetGLFWWindow(), "VkWrapper Test", 800, 600);
 *
 * uint32_t textureId;
 * vkWrapper.AddTexture("exemple.png", textureId);
 *
 * uint32_t modelId;
 * vkWrapper.AddModel("exemple.obj", modelId);
 *
 * vkWrapper.BinTexture(modelId, textureId);
 *
 * vkWrapper.AddShader(SHADER_DIR "vert.spv", "main", VkWrapper::ShaderType::VERTEX);
 * vkWrapper.AddShader(SHADER_DIR "frag.spv", "main", VkWrapper::ShaderType::FRAGMENT);
 *
 * vkWrapper.CreatePipeline();
 *
 * window.SetFramebufferSizeCallback((void *) &vkWrapper, ES::Plugin::VkWrapper::ResizeCallback);
 *
 * VkWrapper::PrintConfig();
 * VkWrapper::PrintAvailableExtensions();
 *
 * while (!window.ShouldClose())
 * {
 *     glfwPollEvents();
 *     auto result = vkWrapper.DrawFrame();
 *     if (result == ES::Plugin::Wrapper::Result::NeedResize)
 *         vkWrapper.Resize(window.GetGLFWWindow());
 * }
 *
 * vkWrapper.Destroy();
 * @endcode
 */
class VkWrapper {
  public:
    /**
     * @brief ShaderType enum class.
     *
     * This enum class is used to represent the type of the shader.
     * It can be either VERTEX, FRAGMENT, GEOMETRY, TESSELLATION_CONTROL, or
     * TESSELLATION_EVALUATION.
     */
    enum class ShaderType : uint8_t {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        TESSELLATION_CONTROL,
        TESSELLATION_EVALUATION
    };

  public:
    /**
     * @brief Create the VkWrapper using the Vulkan API.
     *
     * This function creates the VkWrapper using the Vulkan API. It creates the
     * instance, the surface, get the physical device, the logical device, the swap
     * chain, the image views.
     *
     * @param window  The GLFW window to create the VkWrapper for.
     * @param applicationName  The name of the application.
     * @param width  The width of the window.
     * @param height  The height of the window.
     */
    void CreateInstance(GLFWwindow *window, const std::string &applicationName, const uint32_t width,
                        const uint32_t height);

    /**
     * @brief Create the graphics pipeline using the Vulkan API.
     *
     * This function creates the graphics pipeline using the Vulkan API. It creates
     * the graphics pipeline with the specified shaders and textures.
     */
    void CreatePipeline();

    /**
     * @brief Destroy the VkWrapper using the Vulkan API.
     *
     * This function destroys the VkWrapper using the Vulkan API. It destroys the
     * fences, the semaphores, the command buffers, the command pool, the frame
     * buffers, the graphics pipeline, the render pass, the image views, the swap
     * chain, the logical device, the physical device, the surface, and the instance.
     */
    void Destroy();

    /**
     * @brief Add a texture to the VkWrapper and get the texture id.
     *
     * @param texturePath  The path to the texture.
     * @param textureId  The id of the texture.
     */
    void AddTexture(const std::string &texturePath, uint32_t &textureId);

    /**
     * @brief Add a 3D model to the VkWrapper and get the model id.
     *
     * @param modelPath  The path to the model.
     * @param modelId  The id of the model.
     */
    void AddModel(const std::string &modelPath, uint32_t &modelId);

    /**
     * @brief Bind a texture to a model using the texture id and the model id.
     *
     * @param textureId  The id of the texture.
     * @param modelId  The id of the model.
     */
    void AddModel(const Object::Component::Mesh &model, const std::string &modelName, uint32_t &modelId);

    /**
     * @brief Bind a texture to a model using the texture id and the model id.
     *
     * @param textureId  The id of the texture.
     * @param modelId  The id of the model.
     */
    void BindTexture(const uint32_t textureId, const uint32_t modelId);

    /**
     * @brief Add a shader to the VkWrapper.
     *
     * @note if a shader with the same type is added, the new shader will replace the old one.
     *
     * @param shaderPath  The path to the shader.
     * @param fname  The name of the shader.
     * @param shaderType  The type of the shader.
     */
    void AddShader(const std::string &shaderPath, const std::string &fname, const ShaderType &shaderType);

    /**
     * @brief Draw a frame using the Vulkan API.
     *
     * This function draws a frame using the Vulkan API. It waits for the fences
     * to be signaled, resets the fences, acquires the next image in the swap chain,
     * records the command buffer, submits the command buffer to the graphics queue,
     * presents the image to the screen, and increments the current frame index.
     *
     * @return Wrapper::Result  The result of the draw operation. Success if the
     * image was drawn successfully, NeedResize if a Resize is needed.
     *
     * @see Wrapper::Result
     * @see Resize
     */
    [[nodiscard]] Wrapper::Result DrawFrame();

    /**
     * @brief Resize the window using the Vulkan API.
     *
     * This function resizes the window using the Vulkan API. It gets the new
     * width and height of the window, waits for the window to be resized, and
     * recreates the swap chain with the new dimensions.
     *
     * @param window  The GLFW window to resize.
     */
    void Resize(GLFWwindow *window);

    /**
     * @brief Set the framebuffer resized flag.
     *
     * This function sets the framebuffer resized flag to indicate that the
     * framebuffer needs to be resized.
     * The flag is used to trigger the recreation of the swap chain when the
     * window is resized.
     */
    inline void SetFramebufferResized() { _instance.SetFramebufferResized(true); }

    /**
     * @brief Set the clear color of the VkWrapper.
     *
     * This function sets the clear color of the VkWrapper.
     *
     * @param color  The color to set.
     */
    inline void ChangeClearColor(const glm::vec4 &color) { _instance.SetClearColor(color); }

    /**
     * @brief Callback function for the framebuffer resize event.
     *
     * This function is called when the framebuffer is resized.
     * It sets the framebuffer resized flag to indicate that the framebuffer
     * needs to be resized.
     *
     * @param window  The GLFW window that was resized.
     * @param width  The new width of the window. (unused)
     * @param height  The new height of the window. (unused)
     */
    static void ResizeCallback(GLFWwindow *window, int width, int height);

    /**
     * @brief Print the available extensions for the Vulkan API.
     *
     */
    static void PrintAvailableExtensions();

    /**
     * @brief Print the version of the VkWrapper.
     *
     */
    static void PrintVersion();

    /**
     * @brief Print the configuration of the VkWrapper.
     *
     */
    static void PrintConfig();

  private:
    Wrapper::Instance _instance;
    Wrapper::ShaderModule::ShaderPaths _shaders;
    entt::resource_cache<Wrapper::Texture, Wrapper::TextureLoader> _textures{};
    entt::resource_cache<Object::Component::Mesh, Object::Component::MeshLoader> _models{};
};

} // namespace ES::Plugin

#endif /* !VKWRAPPER_HPP_ */
