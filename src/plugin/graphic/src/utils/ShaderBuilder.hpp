#pragma once

#include "utils/webgpu.hpp"

#include "Logger.hpp"
#include "exception/DuplicatedVertexAttributeLocationError.hpp"
#include "exception/FileReadingError.hpp"
#include "utils/VertexBufferLayout.hpp"
#include "utils/BindGroupLayout.hpp"
#include "utils/IValidable.hpp"

#include <filesystem>
#include <fstream>
#include <list>

namespace Plugin::Graphic::Utils {
class ShaderBuilder : public IValidable {
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
            Log::Error("Shader file does not exist");
            return *this;
        }
        std::ifstream file(path);
        if (!file.is_open())
        {
            Log::Error("Failed to open shader file");
            return *this;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return setShader(buffer.str());
    }

    ShaderBuilder &setVertexEntryPoint(const std::string &entryPoint)
    {
        this->vertexEntryPoint = entryPoint;
        return *this;
    }

    ShaderBuilder &setFragmentEntryPoint(const std::string &entryPoint)
    {
        this->fragmentEntryPoint = entryPoint;
        return *this;
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

    ShaderBuilder &addBindGroupLayout(const BindGroupLayout &layout)
    {
        this->bindGroupLayouts.push_back(layout);
        return *this;
    }

    BindGroupLayout &getBindGroupLayout(size_t index) { return *std::next(this->bindGroupLayouts.begin(), index); }

    std::vector<ValidationError> validate(void) const
    {
        std::vector<ValidationError> errors;
        if (!this->shaderSource.has_value())
        {
            errors.push_back({ "Shader source is not set", "ShaderBuilder", ValidationError::Severity::Error });
        }
        if (this->vertexBufferLayouts.empty())
        {
            errors.push_back({ "No vertex buffer layouts added", "ShaderBuilder", ValidationError::Severity::Error });
        }
        if (!this->vertexEntryPoint.has_value())
        {
            errors.push_back({ "Vertex entry point is not set ('vs_main' will be used)", "ShaderBuilder", ValidationError::Severity::Warning });
        }
        if (!this->fragmentEntryPoint.has_value())
        {
            errors.push_back({ "Fragment entry point is not set ('fs_main' will be used)", "ShaderBuilder", ValidationError::Severity::Warning });
        }
        for (size_t i = 0; i < this->vertexBufferLayouts.size(); i++)
        {
            auto layoutErrors = std::next(this->vertexBufferLayouts.begin(), i)->validate();
            for (const auto &error : layoutErrors)
            {
                errors.push_back({ error.message, fmt::format("ShaderBuilder::({}){}", i, error.location), error.severity });
            }
        }
        for (size_t i = 0; i < this->bindGroupLayouts.size(); i++)
        {
            auto layoutErrors = std::next(this->bindGroupLayouts.begin(), i)->validate();
            for (const auto &error : layoutErrors)
            {
                errors.push_back({ error.message, fmt::format("ShaderBuilder::({}){}", i, error.location), error.severity });
            }
        }
        return errors;
    }

  private:
    std::list<BindGroupLayout> bindGroupLayouts;
    std::list<VertexBufferLayout> vertexBufferLayouts;
    std::optional<std::string> shaderSource;
    std::optional<std::string> fragmentEntryPoint;
    std::optional<std::string> vertexEntryPoint;
};
} // namespace Plugin::Graphic::Utils
