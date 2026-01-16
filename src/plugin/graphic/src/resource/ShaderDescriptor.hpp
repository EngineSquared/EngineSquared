#pragma once

#include "utils/webgpu.hpp"

#include "Logger.hpp"
#include "exception/DuplicatedVertexAttributeLocationError.hpp"
#include "exception/FileReadingError.hpp"
#include "spdlog/fmt/fmt.h"
#include "utils/IValidable.hpp"
#include "utils/shader/BindGroupLayout.hpp"
#include "utils/shader/ColorTargetState.hpp"
#include "utils/shader/DepthStencilState.hpp"
#include "utils/shader/VertexBufferLayout.hpp"

#include <filesystem>
#include <fstream>
#include <list>
#include <optional>

namespace Graphic::Resource {
class ShaderDescriptor : public Utils::IValidable {
  public:
    ShaderDescriptor() = default;
    ~ShaderDescriptor() override = default;

    ShaderDescriptor &setShader(std::string_view source)
    {
        this->_shaderSource = source;
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

    ShaderDescriptor &setVertexEntryPoint(std::string_view entryPoint)
    {
        this->_vertexEntryPoint = entryPoint;
        return *this;
    }

    ShaderDescriptor &setFragmentEntryPoint(std::string_view entryPoint)
    {
        this->_fragmentEntryPoint = entryPoint;
        return *this;
    }

    ShaderDescriptor &addVertexBufferLayout(const Utils::VertexBufferLayout &layout)
    {
        this->_vertexBufferLayouts.push_back(layout);
        return *this;
    }

    Utils::VertexBufferLayout &getVertexBufferLayout(size_t index)
    {
        return *std::next(this->_vertexBufferLayouts.begin(), index);
    }

    ShaderDescriptor &addBindGroupLayout(const Utils::BindGroupLayout &layout)
    {
        this->_bindGroupLayouts.push_back(layout);
        return *this;
    }

    Utils::BindGroupLayout &getBindGroupLayout(size_t index)
    {
        return *std::next(this->_bindGroupLayouts.begin(), index);
    }

    ShaderDescriptor &addOutputColorFormat(const Utils::ColorTargetState &state)
    {
        this->_outputColorFormats.push_back(state);
        return *this;
    }

    ShaderDescriptor &setOutputDepthFormat(const Utils::DepthStencilState &state)
    {
        this->_outputDepthFormat = state;
        return *this;
    }

    ShaderDescriptor &setCullMode(wgpu::CullMode mode)
    {
        this->_cullMode = mode;
        return *this;
    }

    ShaderDescriptor &setPrimitiveTopology(wgpu::PrimitiveTopology topology)
    {
        this->_primitiveTopology = topology;
        return *this;
    }

    ShaderDescriptor &setName(std::string_view name)
    {
        this->_name = name;
        return *this;
    }

    std::vector<Utils::ValidationError> validate(void) const override
    {
        std::vector<Utils::ValidationError> errors;
        if (!this->_shaderSource.has_value())
        {
            errors.emplace_back("Shader source is not set", "ShaderDescriptor",
                                Utils::ValidationError::Severity::Error);
        }
        if (!this->_name.has_value())
        {
            errors.emplace_back("Shader name is not set ('Unnamed' will be used)", "ShaderDescriptor",
                                Utils::ValidationError::Severity::Warning);
        }
        if (this->_vertexBufferLayouts.empty())
        {
            errors.emplace_back("No vertex buffer layouts added", "ShaderDescriptor",
                                Utils::ValidationError::Severity::Warning);
        }
        if (!this->_vertexEntryPoint.has_value())
        {
            errors.emplace_back("Vertex entry point is not set ('vs_main' will be used)", "ShaderDescriptor",
                                Utils::ValidationError::Severity::Warning);
        }
        if (!this->_fragmentEntryPoint.has_value())
        {
            errors.emplace_back("Fragment entry point is not set ('fs_main' will be used)", "ShaderDescriptor",
                                Utils::ValidationError::Severity::Warning);
        }
        for (size_t i = 0; i < this->_vertexBufferLayouts.size(); i++)
        {
            auto layoutErrors = std::next(this->_vertexBufferLayouts.begin(), i)->validate();
            for (const auto &error : layoutErrors)
            {
                errors.emplace_back(error.message, fmt::format("ShaderDescriptor::({}){}", i, error.location),
                                    error.severity);
            }
        }
        for (size_t i = 0; i < this->_bindGroupLayouts.size(); i++)
        {
            auto layoutErrors = std::next(this->_bindGroupLayouts.begin(), i)->validate();
            for (const auto &error : layoutErrors)
            {
                errors.emplace_back(error.message, fmt::format("ShaderDescriptor::({}){}", i, error.location),
                                    error.severity);
            }
        }
        for (size_t i = 0; i < this->_outputColorFormats.size(); i++)
        {
            auto stateErrors = std::next(this->_outputColorFormats.begin(), i)->validate();
            for (const auto &error : stateErrors)
            {
                errors.emplace_back(error.message, fmt::format("ShaderDescriptor::({}){}", i, error.location),
                                    error.severity);
            }
        }
        if (this->_outputDepthFormat.has_value())
        {
            auto stateErrors = this->_outputDepthFormat->validate();
            for (const auto &error : stateErrors)
            {
                errors.emplace_back(error.message, fmt::format("ShaderDescriptor::DepthStencil{}", error.location),
                                    error.severity);
            }
        }

        return errors;
    }

    const std::string &getName() const { return this->_name.has_value() ? this->_name.value() : _DEFAULT_NAME; }
    const std::list<Utils::BindGroupLayout> &getBindGroupLayouts() const { return this->_bindGroupLayouts; }
    const std::list<Utils::VertexBufferLayout> &getVertexBufferLayouts() const { return this->_vertexBufferLayouts; }
    const std::list<Utils::ColorTargetState> &getOutputColorFormats() const { return this->_outputColorFormats; }
    const std::optional<Utils::DepthStencilState> &getOutputDepthFormat() const { return this->_outputDepthFormat; }
    const std::optional<std::string> &getShaderSource() const { return this->_shaderSource; }
    const std::string &getFragmentEntryPoint() const
    {
        return this->_fragmentEntryPoint.has_value() ? this->_fragmentEntryPoint.value() :
                                                       _DEFAULT_FRAGMENT_ENTRY_POINT;
    }
    const std::string &getVertexEntryPoint() const
    {
        return this->_vertexEntryPoint.has_value() ? this->_vertexEntryPoint.value() : _DEFAULT_VERTEX_ENTRY_POINT;
    }
    wgpu::PrimitiveTopology getPrimitiveTopology() const { return this->_primitiveTopology; }
    wgpu::CullMode getCullMode() const { return this->_cullMode; }

  private:
    inline const static std::string _DEFAULT_FRAGMENT_ENTRY_POINT = "fs_main";
    inline const static std::string _DEFAULT_VERTEX_ENTRY_POINT = "vs_main";
    inline const static std::string _DEFAULT_NAME = "Unnamed";

    std::optional<std::string> _name;
    std::list<Utils::BindGroupLayout> _bindGroupLayouts;
    std::list<Utils::VertexBufferLayout> _vertexBufferLayouts;
    std::list<Utils::ColorTargetState> _outputColorFormats;
    std::optional<std::string> _shaderSource;
    std::optional<std::string> _fragmentEntryPoint;
    std::optional<std::string> _vertexEntryPoint;
    std::optional<Utils::DepthStencilState> _outputDepthFormat;
    wgpu::PrimitiveTopology _primitiveTopology = wgpu::PrimitiveTopology::TriangleList;
    wgpu::CullMode _cullMode = wgpu::CullMode::Back;
};
} // namespace Graphic::Resource
