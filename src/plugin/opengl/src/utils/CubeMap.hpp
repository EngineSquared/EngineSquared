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

#include "Logger.hpp"

namespace ES::Plugin::OpenGL::Utils {

/**
 * @brief CubeMap class.
 *
 * This class is used to load .png files for a cube map.
 */
class CubeMap {
  public:
    explicit CubeMap(const std::string &cubeMapPath) { loadFromCross(cubeMapPath); }
    CubeMap(const std::array<std::string, 6> &facesPath) { loadFromFaces(facesPath); }

    ~CubeMap();

    CubeMap(const CubeMap &) = default;
    CubeMap(CubeMap &&) = default;
    CubeMap &operator=(const CubeMap &) = default;
    CubeMap &operator=(CubeMap &&) = default;

    void Bind() const;

  private:
    /**
     * @brief Load a cube map from individual face images.
     *
     * @param path path to the .png files for the cube map faces
     */
    void loadFromFaces(const std::array<std::string, 6> &path);

    /**
     * @brief Load a cube map from a cross image.
     *
     * @param path path to the cross .png file for the cube map
     */
    void loadFromCross(const std::string &path);

  private:
    int _width = 0;
    int _height = 0;
    int _channels = 0;
    GLuint _textureID = 0;
};

} // namespace ES::Plugin::OpenGL::Utils
