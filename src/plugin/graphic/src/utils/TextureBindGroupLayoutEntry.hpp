#pragma once

#include "utils/ABindGroupLayoutEntry.hpp"
#include "utils/webgpu.hpp"

namespace Plugin::Graphic::Utils { // TODO: put this file in the correct forder and update its namespace
class TextureBindGroupLayoutEntry : public ABindGroupLayoutEntry {
  public:
    TextureBindGroupLayoutEntry(const std::string &name) : ABindGroupLayoutEntry(name) {}
    ~TextureBindGroupLayoutEntry() = default;

    virtual std::vector<ValidationError> validate(void) const override
    {
        std::vector<ValidationError> errors = ABindGroupLayoutEntry::validate();
        if (!this->_isSampleTypeSet)
        {
            errors.push_back({"Sample type is not set", fmt::format("TextureBindGroupLayoutEntry({})", this->name),
                              ValidationError::Severity::Error});
        }
        if (!this->_isViewDimensionSet)
        {
            errors.push_back({"View dimension is not set", fmt::format("TextureBindGroupLayoutEntry({})", this->name),
                              ValidationError::Severity::Error});
        }
        return errors;
    }

    inline TextureBindGroupLayoutEntry &setSampleType(wgpu::TextureSampleType type)
    {
        this->entry.texture.sampleType = type;
        this->_isSampleTypeSet = true;
        return *this;
    }

    inline TextureBindGroupLayoutEntry &setViewDimension(wgpu::TextureViewDimension dimension)
    {
        this->entry.texture.viewDimension = dimension;
        this->_isViewDimensionSet = true;
        return *this;
    }

    inline TextureBindGroupLayoutEntry &setMultisampled(bool multisampled)
    {
        this->entry.texture.multisampled = multisampled;
        return *this;
    }

  private:
    bool _isSampleTypeSet = false;
    bool _isViewDimensionSet = false;
};
} // namespace Plugin::Graphic::Utils
