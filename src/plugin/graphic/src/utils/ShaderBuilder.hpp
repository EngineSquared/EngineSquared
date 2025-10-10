#pragma once

#include "utils/webgpu.hpp"

#include "exception/FileReadingError.hpp"
#include "exception/VertexAttributeOverlappingError.hpp"
#include "exception/DuplicatedVertexAttributeLocationError.hpp"

#include <filesystem>
#include <fstream>

namespace Plugin::Graphic::Utils {
class ShaderBuilder {
    public:
        ShaderBuilder() = default;
        ~ShaderBuilder() = default;

        ShaderBuilder& setShader(const std::string &source) {
            this->shaderSource = source;
            return *this;
        }
        ShaderBuilder& setShaderFromFile(const std::filesystem::path &path) {
            if (!std::filesystem::exists(path)) {
                throw Exception::FileReadingError("Shader file does not exist");
            }
            std::ifstream file(path);
            if (!file.is_open()) {
                throw Exception::FileReadingError("Failed to open shader file");
            }
            std::stringstream buffer;
            buffer << file.rdbuf();
            file.close();
            return setShader(buffer.str());
        }

        ShaderBuilder& addVertexAttribute(wgpu::VertexFormat format, uint32_t offset, uint32_t shaderLocation) {
            if (_doShaderLocationExist(shaderLocation)) {
                throw Exception::DuplicatedVertexAttributeLocationError("Shader location " + std::to_string(shaderLocation) + " is already used");
            } else if (auto overlappingAttribute = _getVertexAttributeOverlap(format, offset)) {
                throw Exception::VertexAttributeOverlappingError("New attribute (format: " + std::to_string(static_cast<uint32_t>(format)) + ", offset: " + std::to_string(offset) + ", shaderLocation: " + std::to_string(shaderLocation) + ") overlaps with existing attribute (format: " + std::to_string(static_cast<uint32_t>(overlappingAttribute->format)) + ", offset: " + std::to_string(overlappingAttribute->offset) + ", shaderLocation: " + std::to_string(overlappingAttribute->shaderLocation) + ")");
            }
            wgpu::VertexAttribute attribute;
            attribute.format = format;
            attribute.offset = offset;
            attribute.shaderLocation = shaderLocation;
            this->vertexAttributes.push_back(attribute);
            return *this;
        }

    private:
        inline uint32_t _getVertexFormatSize(wgpu::VertexFormat format) const {
            switch (format) {
                case wgpu::VertexFormat::Float32x2: return 2 * sizeof(float);
                case wgpu::VertexFormat::Float32x3: return 3 * sizeof(float);
                case wgpu::VertexFormat::Float32x4: return 4 * sizeof(float);
                default: throw std::runtime_error("Unknown vertex format");
            }
        }

        inline bool _doShaderLocationExist(uint32_t shaderLocation) const {
            return std::ranges::any_of(this->vertexAttributes, [shaderLocation](const wgpu::VertexAttribute &attr) { return attr.shaderLocation == shaderLocation; });
        }

        inline std::optional<wgpu::VertexAttribute> _getVertexAttributeOverlap(wgpu::VertexFormat format, uint32_t offset) const {
            const uint32_t start = offset;
            const uint32_t end = start + this->_getVertexFormatSize(format);
            for (const auto &attr : this->vertexAttributes) {
                const uint32_t attrStart = attr.offset;
                const uint32_t attrEnd = attrStart + this->_getVertexFormatSize(attr.format);
                if ((start < attrEnd) && (end > attrStart)) {
                    return attr;
                }
            }
            return std::nullopt;
        }

        std::string shaderSource;

        std::vector<wgpu::VertexAttribute> vertexAttributes;
};
}
