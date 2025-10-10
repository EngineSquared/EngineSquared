#pragma once

#include "utils/webgpu.hpp"

#include "exception/DuplicatedVertexAttributeLocationError.hpp"
#include "exception/FileReadingError.hpp"
#include "utils/VertexBufferLayout.hpp"

#include <filesystem>
#include <fstream>
#include <list>

namespace Plugin::Graphic::Utils {
class ShaderBuilder {
  public:
    ShaderBuilder() = default;
    ~ShaderBuilder() = default;

    ShaderBuilder &setShader(const std::string &source)
    {
        this->shaderSource = source;
        return *this;
    }
    ShaderBuilder &setShaderFromFile(const std::filesystem::path &path)
    {
        if (!std::filesystem::exists(path))
        {
            throw Exception::FileReadingError("Shader file does not exist");
        }
        std::ifstream file(path);
        if (!file.is_open())
        {
            throw Exception::FileReadingError("Failed to open shader file");
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return setShader(buffer.str());
    }

    ShaderBuilder &addVertexBufferLayout(const VertexBufferLayout &layout)
    {
        this->vertexBufferLayouts.push_back(layout);
        return *this;
    }

    VertexBufferLayout &getVertexBufferLayout(size_t index)
    {
        return *std::next(this->vertexBufferLayouts.begin(), index);
    }

  private:
    std::list<VertexBufferLayout> vertexBufferLayouts;
    std::string shaderSource;
};
} // namespace Plugin::Graphic::Utils
