#pragma once

#include "utils/webgpu.hpp"
#include "utils/ABindGroupLayoutEntry.hpp"

namespace Plugin::Graphic::Utils { // TODO: put this file in the correct forder and update its namespace
class BufferBindGroupLayoutEntry : public ABindGroupLayoutEntry {
    public:
        BufferBindGroupLayoutEntry(const std::string &name): ABindGroupLayoutEntry(name) { }
        ~BufferBindGroupLayoutEntry() = default;

        virtual bool isComplete() const override {
            return ABindGroupLayoutEntry::isComplete() && this->isTypeSet && this->isMinBindingSizeSet;
        }

        inline BufferBindGroupLayoutEntry &setType(wgpu::BufferBindingType type) {
            this->entry.buffer.type = type;
            this->isTypeSet = true;
            return *this;
        }

        template<typename ...TBuffer>
        inline BufferBindGroupLayoutEntry &setType(void) {
            uint64_t size = 0;
            (computeMinBindingSize<TBuffer>(size), ...);
            this->entry.buffer.minBindingSize = size;
            this->isMinBindingSizeSet = true;
            return *this;
        }

        inline BufferBindGroupLayoutEntry &setHasDynamicOffset(bool hasDynamicOffset) {
            this->entry.buffer.hasDynamicOffset = hasDynamicOffset;
            return *this;
        }

    private:
        bool isTypeSet = false;
        bool isMinBindingSizeSet = false;

        private:
        template<typename T>
        void computeMinBindingSize(uint64_t &size) {
            if (sizeof(T) < 16) {
                size += 16;
            } else {
                size += sizeof(T);
            }
        }
    };
}
