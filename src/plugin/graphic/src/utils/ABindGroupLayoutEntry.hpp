#pragma once

#include "utils/IBindGroupLayoutEntry.hpp"
#include "utils/webgpu.hpp"

namespace Plugin::Graphic::Utils { // TODO: put this file in the correct forder and update its namespace

template <typename TDerived> class ABindGroupLayoutEntry : public IBindGroupLayoutEntry {
  public:
    explicit ABindGroupLayoutEntry(const std::string &name_) : name(name_)
    {
        this->entry.buffer.type = wgpu::BufferBindingType::BindingNotUsed;
        this->entry.sampler.type = wgpu::SamplerBindingType::BindingNotUsed;
        this->entry.texture.sampleType = wgpu::TextureSampleType::BindingNotUsed;
        this->entry.storageTexture.access = wgpu::StorageTextureAccess::BindingNotUsed;
    }
    virtual ~ABindGroupLayoutEntry() = default;

    inline const std::string &getName() const override { return this->name; }

    inline const wgpu::BindGroupLayoutEntry &getEntry() const override { return this->entry; }

    inline TDerived &setBinding(uint32_t binding)
    {
        this->entry.binding = binding;
        this->isBindingSet = true;
        return static_cast<TDerived &>(*this);
    }

    inline TDerived &setVisibility(wgpu::ShaderStage visibility)
    {
        this->entry.visibility = visibility;
        this->isVisibilitySet = true;
        return static_cast<TDerived &>(*this);
    }

    virtual std::vector<ValidationError> validate(void) const override
    {
        std::vector<ValidationError> errors;
        if (!this->isBindingSet)
        {
            errors.push_back(
                {"Binding is not set", "BindGroupLayoutEntry: " + this->name, ValidationError::Severity::Error});
        }
        if (!this->isVisibilitySet)
        {
            errors.push_back(
                {"Visibility is not set", "BindGroupLayoutEntry: " + this->name, ValidationError::Severity::Error});
        }
        return errors;
    }

  protected:
    wgpu::BindGroupLayoutEntry entry = wgpu::BindGroupLayoutEntry(wgpu::Default);
    bool isBindingSet = false;
    bool isVisibilitySet = false;
    std::string name;
};
} // namespace Plugin::Graphic::Utils
