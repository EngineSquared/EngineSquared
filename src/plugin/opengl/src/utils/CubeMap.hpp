/**************************************************************************
 * EngineSquared v0.0.0
 *
 * EngineSquared is a software package, part of the Engine².
 *
 * This file is part of the EngineSquared project that is under GPL-3.0 License.
 * Copyright © 2024 by @EngineSquared, All rights reserved.
 *
 * EngineSquared is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file CubeMap.hpp
 * @brief CubeMap class declaration.
 *
 * This class is used to load .png files for a cube map.
 * It provides methods to load cube maps from a cross image or from individual face images.
 * It also provides a method to bind the cube map texture for rendering.
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2025-06-15
 **************************************************************************/

#pragma once

#include <GL/glew.h>
#include <array>
#include <memory>
#include <optional>
#include <string_view>

#include "Logger.hpp"

namespace ES::Plugin::OpenGL::Utils {

/**
 * @brief Modern CubeMap texture loader and manager.
 *
 * This class provides RAII management for OpenGL cubemap textures.
 * Supports loading from individual face images or cross-layout images.
 *
 * @note This class is non-copyable but movable for performance.
 *
 * @example
 * @code
 * // Load from cross image
 * CubeMap cubemap("assets/skybox_cross.png");
 * if (cubemap.IsValid()) {
 *     cubemap.Bind();
 * }
 *
 * // Load from individual faces
 * std::array<std::string, 6> faces = {"right.jpg", "left.jpg", ...};
 * CubeMap cubemap2(faces);
 * @endcode
 */
class CubeMap {
  public:
    /**
     * @brief Face enumeration for cubemap faces.
     */
    enum class CubeFace : std::uint8_t {
        PositiveX = 0, ///< Right face
        NegativeX = 1, ///< Left face
        PositiveY = 2, ///< Top face
        NegativeY = 3, ///< Bottom face
        PositiveZ = 4, ///< Front face
        NegativeZ = 5, ///< Back face
        Count = 6
    };

    /**
     * @brief Create a CubeMap from a cross-layout image.
     *
     * @param cubeMapPath Path to the cross-layout image (4:3 aspect ratio)
     */
    explicit CubeMap(std::string_view cubeMapPath);

    /**
     * @brief Create a CubeMap from individual face images.
     *
     * @param facesPath Array of 6 paths in order: +X, -X, +Y, -Y, +Z, -Z
     */
    explicit CubeMap(const std::array<std::string, 6> &facesPath);

    /**
     * @brief Destructor - automatically cleans up OpenGL resources.
     */
    ~CubeMap() noexcept;

    // Non-copyable but movable
    CubeMap(const CubeMap &) = delete;
    CubeMap &operator=(const CubeMap &) = delete;
    CubeMap(CubeMap &&) noexcept;
    CubeMap &operator=(CubeMap &&) noexcept;

    /**
     * @brief Bind the cubemap texture to GL_TEXTURE0 or a specific texture unit.
     *
     * @param textureUnit Texture unit (0-31), defaults to 0
     *
     * @note Does nothing if texture is invalid.
     */
    void Bind(std::uint32_t textureUnit = 0) const noexcept;

    /**
     * @brief Check if the cubemap is valid.
     *
     * @return true if texture ID is valid
     */
    [[nodiscard]] constexpr bool IsValid() const noexcept { return _textureID != 0; }

    /**
     * @brief Get the OpenGL texture ID.
     *
     * @return GLuint texture ID (0 if invalid)
     */
    [[nodiscard]] constexpr GLuint GetTextureID() const noexcept { return _textureID; }

    /**
     * @brief Get cubemap dimensions.
     *
     * @return std::pair<int, int> width and height
     */
    [[nodiscard]] constexpr std::pair<int, int> GetDimensions() const noexcept { return {_width, _height}; }

  private:
    /**
     * @brief Load cubemap from individual face images.
     *
     * @param faces Array of face image paths
     * @return bool true on success
     */
    bool LoadFromFaces(const std::array<std::string, 6> &faces) noexcept;

    /**
     * @brief Load cubemap from cross-layout image.
     *
     * @param path Path to cross image
     * @return bool true on success
     */
    bool LoadFromCross(std::string_view path) noexcept;

    /**
     * @brief Setup texture parameters for the cubemap.
     */
    void SetupTextureParameters() noexcept;

    /**
     * @brief Clean up OpenGL resources.
     */
    void Cleanup() noexcept;

  private:
    int _width{0};        ///< Texture width
    int _height{0};       ///< Texture height
    int _channels{0};     ///< Number of color channels
    GLuint _textureID{0}; ///< OpenGL texture ID
};

} // namespace ES::Plugin::OpenGL::Utils
