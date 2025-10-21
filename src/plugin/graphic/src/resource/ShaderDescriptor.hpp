#pragma once

#include "utils/webgpu.hpp"

#include "Logger.hpp"
#include "exception/DuplicatedVertexAttributeLocationError.hpp"
#include "exception/FileReadingError.hpp"
#include "spdlog/fmt/fmt.h"
#include "utils/BindGroupLayout.hpp"
#include "utils/ColorTargetState.hpp"
#include "utils/DepthStencilState.hpp"
#include "utils/IValidable.hpp"
#include "utils/VertexBufferLayout.hpp"

#include <filesystem>
#include <fstream>
#include <list>
#include <optional>

namespace Plugin::Graphic::Resource {
class ShaderDescriptor : public Utils::IValidable {
  public:

    ShaderDescriptor() = default;
    ~ShaderDescriptor() = default;

    ShaderDescriptor &setShader(const std::string &source)
    {
        this->shaderSource = source;
        return *this;
    }
    ShaderDescriptor &setShaderFromFile(const std::filesystem::path &path)
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

    ShaderDescriptor &setVertexEntryPoint(const std::string &entryPoint)
    {
        this->vertexEntryPoint = entryPoint;
        return *this;
    }

    ShaderDescriptor &setFragmentEntryPoint(const std::string &entryPoint)
    {
        this->fragmentEntryPoint = entryPoint;
        return *this;
    }

    ShaderDescriptor &addVertexBufferLayout(const Utils::VertexBufferLayout &layout)
    {
        this->vertexBufferLayouts.push_back(layout);
        return *this;
    }

    Utils::VertexBufferLayout &getVertexBufferLayout(size_t index)
    {
        return *std::next(this->vertexBufferLayouts.begin(), index);
    }

    ShaderDescriptor &addBindGroupLayout(const Utils::BindGroupLayout &layout)
    {
        this->bindGroupLayouts.push_back(layout);
        return *this;
    }

    Utils::BindGroupLayout &getBindGroupLayout(size_t index) { return *std::next(this->bindGroupLayouts.begin(), index); }

    ShaderDescriptor &addOutputColorFormat(const Utils::ColorTargetState &state)
    {
        this->outputColorFormats.push_back(state);
        return *this;
    }

    ShaderDescriptor &setOutputDepthFormat(const Utils::DepthStencilState &state)
    {
        this->outputDepthFormat = state;
        return *this;
    }

    ShaderDescriptor &setCullMode(wgpu::CullMode mode)
    {
        this->cullMode = mode;
        return *this;
    }

    ShaderDescriptor &setPrimitiveTopology(wgpu::PrimitiveTopology topology)
    {
        this->primitiveTopology = topology;
        return *this;
    }

    ShaderDescriptor &setName(const std::string &name)
    {
        this->name = name;
        return *this;
    }

    std::vector<Utils::ValidationError> validate(void) const
    {
        std::vector<Utils::ValidationError> errors;
        if (!this->shaderSource.has_value())
        {
            errors.push_back({"Shader source is not set", "ShaderDescriptor", Utils::ValidationError::Severity::Error});
        }
        if (!this->name.has_value())
        {
            errors.push_back({"Shader name is not set ('Unnamed' will be used)", "ShaderDescriptor",
                              Utils::ValidationError::Severity::Warning});
        }
        if (this->vertexBufferLayouts.empty())
        {
            errors.push_back({"No vertex buffer layouts added", "ShaderDescriptor", Utils::ValidationError::Severity::Error});
        }
        if (!this->vertexEntryPoint.has_value())
        {
            errors.push_back({"Vertex entry point is not set ('vs_main' will be used)", "ShaderDescriptor",
                              Utils::ValidationError::Severity::Warning});
        }
        if (!this->fragmentEntryPoint.has_value())
        {
            errors.push_back({"Fragment entry point is not set ('fs_main' will be used)", "ShaderDescriptor",
                              Utils::ValidationError::Severity::Warning});
        }
        for (size_t i = 0; i < this->vertexBufferLayouts.size(); i++)
        {
            auto layoutErrors = std::next(this->vertexBufferLayouts.begin(), i)->validate();
            for (const auto &error : layoutErrors)
            {
                errors.push_back(
                    {error.message, fmt::format("ShaderDescriptor::({}){}", i, error.location), error.severity});
            }
        }
        for (size_t i = 0; i < this->bindGroupLayouts.size(); i++)
        {
            auto layoutErrors = std::next(this->bindGroupLayouts.begin(), i)->validate();
            for (const auto &error : layoutErrors)
            {
                errors.push_back(
                    {error.message, fmt::format("ShaderDescriptor::({}){}", i, error.location), error.severity});
            }
        }
        for (size_t i = 0; i < this->outputColorFormats.size(); i++)
        {
            auto stateErrors = std::next(this->outputColorFormats.begin(), i)->validate();
            for (const auto &error : stateErrors)
            {
                errors.push_back(
                    {error.message, fmt::format("ShaderDescriptor::({}){}", i, error.location), error.severity});
            }
        }
        if (this->outputDepthFormat.has_value())
        {
            auto stateErrors = this->outputDepthFormat->validate();
            for (const auto &error : stateErrors)
            {
                errors.push_back(
                    {error.message, fmt::format("ShaderDescriptor::DepthStencil{}", error.location), error.severity});
            }
        }
        return errors;
    }

    const std::string& getName() const { return this->name.has_value() ? this->name.value() : DEFAULT_NAME; }
    const std::list<Utils::BindGroupLayout> &getBindGroupLayouts() const { return this->bindGroupLayouts; }
    const std::list<Utils::VertexBufferLayout> &getVertexBufferLayouts() const { return this->vertexBufferLayouts; }
    const std::list<Utils::ColorTargetState> &getOutputColorFormats() const { return this->outputColorFormats; }
    const std::optional<Utils::DepthStencilState> &getOutputDepthFormat() const { return this->outputDepthFormat; }
    const std::optional<std::string> &getShaderSource() const { return this->shaderSource; }
    const std::string &getFragmentEntryPoint() const { return this->fragmentEntryPoint.has_value() ? this->fragmentEntryPoint.value() : DEFAULT_FRAGMENT_ENTRY_POINT; }
    const std::string &getVertexEntryPoint() const { return this->vertexEntryPoint.has_value() ? this->vertexEntryPoint.value() : DEFAULT_VERTEX_ENTRY_POINT; }
    wgpu::PrimitiveTopology getPrimitiveTopology() const { return this->primitiveTopology; }
    wgpu::CullMode getCullMode() const { return this->cullMode; }

    private:
        inline const static std::string DEFAULT_FRAGMENT_ENTRY_POINT = "fs_main";
        inline const static std::string DEFAULT_VERTEX_ENTRY_POINT = "vs_main";
        inline const static std::string DEFAULT_NAME = "Unnamed";

        std::optional<std::string> name;
        std::list<Utils::BindGroupLayout> bindGroupLayouts;
        std::list<Utils::VertexBufferLayout> vertexBufferLayouts;
        std::list<Utils::ColorTargetState> outputColorFormats;
        std::optional<std::string> shaderSource;
        std::optional<std::string> fragmentEntryPoint;
        std::optional<std::string> vertexEntryPoint;
        std::optional<Utils::DepthStencilState> outputDepthFormat;
        wgpu::PrimitiveTopology primitiveTopology = wgpu::PrimitiveTopology::TriangleList;
        wgpu::CullMode cullMode = wgpu::CullMode::None;
};
} // namespace Plugin::Graphic::Utils
