#pragma once

#include "utils/ABindGroupLayoutEntry.hpp"
#include "utils/IValidable.hpp"
#include "spdlog/fmt/fmt.h"
#include <vector>

namespace Plugin::Graphic::Utils { // TODO: put this file in the correct forder and update its namespace
class BindGroupLayout : public IValidable {
  public:
    BindGroupLayout(const std::string &name) : name(name) {}
    ~BindGroupLayout() = default;

    BindGroupLayout &addEntry(const ABindGroupLayoutEntry &entry)
    {
        this->entries.push_back(entry);
        return *this;
    }

    std::vector<ValidationError> validate(void) const override
    {
        std::vector<ValidationError> errors;
        if (this->entries.empty())
        {
            errors.push_back({ "No entries in the bind group layout", fmt::format("BindGroupLayout({})", this->name), ValidationError::Severity::Warning });
            return errors;
        }
        for (const auto &entry : this->entries)
        {
            auto entryErrors = entry.validate();
            for (const auto &error : entryErrors)
            {
                errors.push_back({ error.message, fmt::format("BindGroupLayout::{}::{}", error.location, entry.name), error.severity });
            }
        }
        for (uint32_t i = 0; i < this->entries.size(); i++)
        {
            const auto &entry = this->entries[i];
            for (uint32_t j = i + 1; j < this->entries.size(); j++)
            {
                const auto &otherEntry = this->entries[j];
                if (entry.getEntry().binding == otherEntry.getEntry().binding)
                {
                    errors.push_back({
                      "Binding " + std::to_string(entry.getEntry().binding) + " is duplicated between entries '" + entry.name + "' and '" + otherEntry.name + "'",
                      fmt::format("BindGroupLayout({})", this->name),
                      ValidationError::Severity::Error });
                }
                if (entry.name == otherEntry.name)
                {
                    errors.push_back({
                      "Entry name '" + entry.name + "' is duplicated between entries at index " + std::to_string(i) + " and " + std::to_string(j),
                      fmt::format("BindGroupLayout({})", this->name),
                      ValidationError::Severity::Warning });
                }
            }
        }
        return errors;
    }

  private:
    std::vector<ABindGroupLayoutEntry> entries;
    std::string name;
};
} // namespace Plugin::Graphic::Utils
