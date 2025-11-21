#pragma once

#include "utils/shader/ABindGroupLayoutEntry.hpp"
#include "utils/webgpu.hpp"

namespace Graphic::Utils {
class BufferBindGroupLayoutEntry : public ABindGroupLayoutEntry<BufferBindGroupLayoutEntry> {
  public:
    explicit BufferBindGroupLayoutEntry(const std::string &name) : ABindGroupLayoutEntry(name)
    {
        this->getEntry().buffer.type = wgpu::BufferBindingType::Undefined;
    }
    ~BufferBindGroupLayoutEntry() override = default;

    BufferBindGroupLayoutEntry(const BufferBindGroupLayoutEntry &other) = default;
    BufferBindGroupLayoutEntry &operator=(const BufferBindGroupLayoutEntry &other) = default;

    std::vector<ValidationError> validate(void) const override
    {
        std::vector<ValidationError> errors = ABindGroupLayoutEntry::validate();
        if (!this->isTypeSet)
        {
            errors.emplace_back("Type is not set", fmt::format("BufferBindGroupLayoutEntry({})", this->getName()),
                                ValidationError::Severity::Error);
        }
        if (!this->isMinBindingSizeSet)
        {
            errors.emplace_back("Min binding size is not set",
                                fmt::format("BufferBindGroupLayoutEntry({})", this->getName()),
                                ValidationError::Severity::Warning);
        }
        return errors;
    }

    inline BufferBindGroupLayoutEntry &setType(wgpu::BufferBindingType type)
    {
        this->getEntry().buffer.type = type;
        this->isTypeSet = true;
        return *this;
    }

    template <typename... TBuffer> inline BufferBindGroupLayoutEntry &setMinBindingSize(void)
    {
        uint64_t size = 0;
        (computeMinBindingSize<TBuffer>(size), ...);
        this->getEntry().buffer.minBindingSize = size;
        this->isMinBindingSizeSet = true;
        return *this;
    }

    inline BufferBindGroupLayoutEntry &setMinBindingSize(uint64_t size)
    {
        this->getEntry().buffer.minBindingSize = size;
        this->isMinBindingSizeSet = true;
        return *this;
    }

    inline BufferBindGroupLayoutEntry &setHasDynamicOffset(bool hasDynamicOffset)
    {
        this->getEntry().buffer.hasDynamicOffset = hasDynamicOffset;
        return *this;
    }

  private:
    template <typename T> void computeMinBindingSize(uint64_t &size)
    {
        size += sizeof(T);
    }

    bool isTypeSet = false;
    bool isMinBindingSizeSet = false;
};
} // namespace Graphic::Utils
