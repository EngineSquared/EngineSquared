#pragma once

#include "utils/shader/ABindGroupLayoutEntry.hpp"
#include "utils/webgpu.hpp"

namespace Graphic::Utils {
class TextureBindGroupLayoutEntry : public ABindGroupLayoutEntry<TextureBindGroupLayoutEntry> {
  public:
    TextureBindGroupLayoutEntry(const std::string &name) : ABindGroupLayoutEntry(name)
    {
        this->getEntry().texture.sampleType = wgpu::TextureSampleType::Undefined;
    }
    ~TextureBindGroupLayoutEntry() = default;

    TextureBindGroupLayoutEntry(const TextureBindGroupLayoutEntry &other) = default;
    TextureBindGroupLayoutEntry &operator=(const TextureBindGroupLayoutEntry &other) = default;

    std::vector<ValidationError> validate(void) const override
    {
        std::vector<ValidationError> errors = ABindGroupLayoutEntry::validate();
        if (!this->_isSampleTypeSet)
        {
            errors.emplace_back("Sample type is not set", fmt::format("TextureBindGroupLayoutEntry({})", this->getName()),
                              ValidationError::Severity::Error);
        }
        if (!this->_isViewDimensionSet)
        {
            errors.emplace_back("View dimension is not set", fmt::format("TextureBindGroupLayoutEntry({})", this->getName()),
                              ValidationError::Severity::Error);
        }
        return errors;
    }

    inline TextureBindGroupLayoutEntry &setSampleType(wgpu::TextureSampleType type)
    {
        this->getEntry().texture.sampleType = type;
        this->_isSampleTypeSet = true;
        return *this;
    }

    inline TextureBindGroupLayoutEntry &setViewDimension(wgpu::TextureViewDimension dimension)
    {
        this->getEntry().texture.viewDimension = dimension;
        this->_isViewDimensionSet = true;
        return *this;
    }

    inline TextureBindGroupLayoutEntry &setMultisampled(bool multisampled)
    {
        this->getEntry().texture.multisampled = multisampled;
        return *this;
    }

  private:
    bool _isSampleTypeSet = false;
    bool _isViewDimensionSet = false;
};
} // namespace Graphic::Utils
