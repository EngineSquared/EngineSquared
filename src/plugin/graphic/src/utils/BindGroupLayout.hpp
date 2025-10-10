#pragma once

#include "utils/ABindGroupLayoutEntry.hpp"

namespace Plugin::Graphic::Utils { // TODO: put this file in the correct forder and update its namespace
class BindGroupLayout {
    public:
        BindGroupLayout(void) = default;
        ~BindGroupLayout() = default;

        BindGroupLayout &addEntry(const ABindGroupLayoutEntry &entry) {
            if (!entry.isComplete()) {
                throw std::runtime_error("BindGroupLayoutEntry is not complete"); // TODO: create a specific exception
            }
            this->entries.push_back(entry);
            return *this;
        }

    private:
        std::vector<ABindGroupLayoutEntry> entries;
};
}
