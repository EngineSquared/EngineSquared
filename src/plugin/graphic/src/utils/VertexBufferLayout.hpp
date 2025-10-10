#pragma once

#include "utils/webgpu.hpp"

#include "exception/VertexAttributeOverlappingError.hpp"
#include "exception/DuplicatedVertexAttributeLocationError.hpp"
#include "exception/UnknownFormatType.hpp"

namespace Plugin::Graphic::Utils {
class VertexBufferLayout {
    public:
        VertexBufferLayout() = default;
        ~VertexBufferLayout() = default;

        VertexBufferLayout& addVertexAttribute(wgpu::VertexFormat format, uint32_t offset, uint32_t shaderLocation) {
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

        VertexBufferLayout& setArrayStride(uint32_t stride) {
            this->arrayStride = stride;
            return *this;
        }

        VertexBufferLayout& setStepMode(wgpu::VertexStepMode mode) {
            this->stepMode = mode;
            return *this;
        }

        const std::optional<uint32_t> &getArrayStride() const {
            return this->arrayStride;
        }

        wgpu::VertexStepMode getStepMode() const {
            return this->stepMode;
        }

        const std::vector<wgpu::VertexAttribute>& getVertexAttributes() const {
            return this->vertexAttributes;
        }

    private:
        inline uint32_t _getVertexFormatSize(wgpu::VertexFormat format) const {
            switch (format) {
                case wgpu::VertexFormat::Float32x2: return 2 * sizeof(float);
                case wgpu::VertexFormat::Float32x3: return 3 * sizeof(float);
                case wgpu::VertexFormat::Float32x4: return 4 * sizeof(float);
                default: throw Exception::UnknownFormatType("Unknown vertex format"); // TODO: create a specific exception
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
    std::vector<wgpu::VertexAttribute> vertexAttributes;
    std::optional<uint32_t> arrayStride;
    wgpu::VertexStepMode stepMode = wgpu::VertexStepMode::Vertex;
};
}

