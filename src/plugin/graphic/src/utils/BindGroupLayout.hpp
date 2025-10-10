#pragma once

#include "utils/ABindGroupLayoutEntry.hpp"

namespace Plugin::Graphic::Utils { // TODO: put this file in the correct forder and update its namespace
class BindGroupLayout {
  public:
    BindGroupLayout(const std::string &name) : name(name) {}
    ~BindGroupLayout() = default;

    BindGroupLayout &addEntry(const ABindGroupLayoutEntry &entry)
    {
        this->entries.push_back(entry);
        return *this;
    }

  private:
    std::vector<ABindGroupLayoutEntry> entries;
    std::string name;
};
} // namespace Plugin::Graphic::Utils
