#pragma once

#include "utils/webgpu.hpp"

#include "exception/DuplicatedVertexAttributeLocationError.hpp"
#include "exception/UnknownFormatType.hpp"
#include "exception/VertexAttributeOverlappingError.hpp"
#include "utils/IValidable.hpp"

namespace Graphic::Utils {
class VertexBufferLayout : public IValidable {
  public:
    VertexBufferLayout() = default;
    ~VertexBufferLayout() = default;

    VertexBufferLayout &addVertexAttribute(wgpu::VertexFormat format, uint32_t offset, uint32_t shaderLocation)
    {
        wgpu::VertexAttribute attribute;
        attribute.format = format;
        attribute.offset = offset;
        attribute.shaderLocation = shaderLocation;
        this->vertexAttributes.push_back(attribute);
        return *this;
    }

    VertexBufferLayout &setArrayStride(uint32_t stride)
    {
        this->arrayStride = stride;
        return *this;
    }

    VertexBufferLayout &setStepMode(wgpu::VertexStepMode mode)
    {
        this->stepMode = mode;
        return *this;
    }

    inline uint32_t getArrayStride() const { return this->arrayStride.value_or(_computeArrayStride()); }

    inline wgpu::VertexStepMode getStepMode() const { return this->stepMode; }

    inline const std::vector<wgpu::VertexAttribute> &getVertexAttributes() const { return this->vertexAttributes; }

    std::vector<ValidationError> validate(void) const override
    {
        std::vector<ValidationError> errors;
        if (!this->arrayStride.has_value())
        {
            errors.push_back({"Array stride is not set (auto computation will be used)", "VertexBufferLayout",
                              ValidationError::Severity::Warning});
        }
        if (auto duplicatedLocations = this->_getDuplicatedShaderLocation(); !duplicatedLocations.empty())
        {
            for (const auto &[i, j] : duplicatedLocations)
            {
                errors.push_back({"Shader location " + std::to_string(this->vertexAttributes[i].shaderLocation) +
                                      " is duplicated between attributes at index " + std::to_string(i) + " and " +
                                      std::to_string(j),
                                  "VertexBufferLayout", ValidationError::Severity::Error});
            }
        }
        if (auto overlappingAttribute = this->_getOverlappingVertexAttributes(); !overlappingAttribute.empty())
        {
            for (const auto &[i, j] : overlappingAttribute)
            {
                errors.push_back(
                    {"Attribute at index " + std::to_string(i) +
                         " (format: " + std::to_string(static_cast<uint32_t>(this->vertexAttributes[i].format)) +
                         ", offset: " + std::to_string(this->vertexAttributes[i].offset) +
                         ", shaderLocation: " + std::to_string(this->vertexAttributes[i].shaderLocation) +
                         ") overlaps with attribute at index " + std::to_string(j) +
                         " (format: " + std::to_string(static_cast<uint32_t>(this->vertexAttributes[j].format)) +
                         ", offset: " + std::to_string(this->vertexAttributes[j].offset) +
                         ", shaderLocation: " + std::to_string(this->vertexAttributes[j].shaderLocation) + ")",
                     "VertexBufferLayout", ValidationError::Severity::Error});
            }
        }
        return errors;
    }

  private:
    inline uint32_t _computeArrayStride() const
    {
        uint32_t maxEnd = 0;
        for (const auto &attribute : this->vertexAttributes)
        {
            const uint32_t attributeEnd = attribute.offset + this->_getVertexFormatSize(attribute.format);
            if (attributeEnd > maxEnd)
            {
                maxEnd = attributeEnd;
            }
        }
        return maxEnd;
    }

    inline uint32_t _getVertexFormatSize(wgpu::VertexFormat format) const
    {
        switch (format)
        {
        case wgpu::VertexFormat::Float32x2: return 2 * sizeof(float);
        case wgpu::VertexFormat::Float32x3: return 3 * sizeof(float);
        case wgpu::VertexFormat::Float32x4: return 4 * sizeof(float);
        default: throw Exception::UnknownFormatType("Unknown vertex format"); // TODO: create a specific exception
        }
    }

    inline std::vector<std::pair<uint64_t, uint64_t>> _getDuplicatedShaderLocation(void) const
    {
        std::vector<std::pair<uint64_t, uint64_t>> duplicatedIndices;
        for (size_t i = 0; i < this->vertexAttributes.size(); i++)
        {
            for (size_t j = i + 1; j < this->vertexAttributes.size(); j++)
            {
                if (this->vertexAttributes[i].shaderLocation == this->vertexAttributes[j].shaderLocation)
                {
                    duplicatedIndices.push_back(std::make_pair(i, j));
                }
            }
        }
        return duplicatedIndices;
    }

    inline std::vector<std::pair<uint64_t, uint64_t>> _getOverlappingVertexAttributes(void) const
    {
        std::vector<std::pair<uint64_t, uint64_t>> overlappingIndices;
        for (size_t i = 0; i < this->vertexAttributes.size(); i++)
        {
            for (size_t j = i + 1; j < this->vertexAttributes.size(); j++)
            {
                if (this->_doVertexAttributeOverlap(this->vertexAttributes[i], this->vertexAttributes[j]))
                {
                    overlappingIndices.push_back(std::make_pair(i, j));
                }
            }
        }
        return overlappingIndices;
    }

    inline bool _doVertexAttributeOverlap(const wgpu::VertexAttribute &firstAttribute,
                                          const wgpu::VertexAttribute &secondAttribute) const
    {
        const uint32_t firstStart = firstAttribute.offset;
        const uint32_t firstEnd = firstStart + this->_getVertexFormatSize(firstAttribute.format);
        const uint32_t secondStart = secondAttribute.offset;
        const uint32_t secondEnd = secondStart + this->_getVertexFormatSize(secondAttribute.format);
        if ((firstStart < secondEnd) && (firstEnd > secondStart))
        {
            return true;
        }
        return false;
    }

    std::vector<wgpu::VertexAttribute> vertexAttributes;
    std::optional<uint32_t> arrayStride;
    wgpu::VertexStepMode stepMode = wgpu::VertexStepMode::Vertex;
};
} // namespace Graphic::Utils
