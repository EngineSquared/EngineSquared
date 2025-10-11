#pragma once

#include "utils/ABindGroupLayoutEntry.hpp"
#include "utils/webgpu.hpp"

namespace Plugin::Graphic::Utils { // TODO: put this file in the correct forder and update its namespace
class BufferBindGroupLayoutEntry : public ABindGroupLayoutEntry {
  public:
    BufferBindGroupLayoutEntry(const std::string &name) : ABindGroupLayoutEntry(name) {}
    ~BufferBindGroupLayoutEntry() = default;

    virtual std::vector<ValidationError> validate(void) const override
    {
        std::vector<ValidationError> errors = ABindGroupLayoutEntry::validate();
        if (!this->isTypeSet)
        {
            errors.push_back({ "Type is not set", fmt::format("BufferBindGroupLayoutEntry({})", this->name), ValidationError::Severity::Error });
        }
        if (!this->isMinBindingSizeSet)
        {
            errors.push_back({ "Min binding size is not set", fmt::format("BufferBindGroupLayoutEntry({})", this->name), ValidationError::Severity::Warning });
        }
        return errors;
    }

    inline BufferBindGroupLayoutEntry &setType(wgpu::BufferBindingType type)
    {
        this->entry.buffer.type = type;
        this->isTypeSet = true;
        return *this;
    }

    template <typename... TBuffer> inline BufferBindGroupLayoutEntry &setType(void)
    {
        uint64_t size = 0;
        (computeMinBindingSize<TBuffer>(size), ...);
        this->entry.buffer.minBindingSize = size;
        this->isMinBindingSizeSet = true;
        return *this;
    }

    inline BufferBindGroupLayoutEntry &setMinBindingSize(uint64_t size)
    {
        this->entry.buffer.minBindingSize = size;
        this->isMinBindingSizeSet = true;
        return *this;
    }

    inline BufferBindGroupLayoutEntry &setHasDynamicOffset(bool hasDynamicOffset)
    {
        this->entry.buffer.hasDynamicOffset = hasDynamicOffset;
        return *this;
    }

  private:
    bool isTypeSet = false;
    bool isMinBindingSizeSet = false;

  private:
    template <typename T> void computeMinBindingSize(uint64_t &size)
    {
        if (sizeof(T) < 16)
        {
            size += 16;
        }
        else
        {
            size += sizeof(T);
        }
    }
};
} // namespace Plugin::Graphic::Utils
