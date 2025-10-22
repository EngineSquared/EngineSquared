#pragma once

#include "spdlog/fmt/fmt.h"
#include "utils/ABindGroupLayoutEntry.hpp"
#include "utils/IValidable.hpp"
#include <concepts>
#include <vector>

template <typename TBindGroupLayoutEntry>
concept CBindGroupLayoutEntry = std::derived_from<TBindGroupLayoutEntry, Graphic::Utils::IBindGroupLayoutEntry>;

namespace Graphic::Utils { // TODO: put this file in the correct forder and update its namespace
class BindGroupLayout : public IValidable {
  public:
    BindGroupLayout(const std::string &name) : name(name) {}
    ~BindGroupLayout() = default;

    template <CBindGroupLayoutEntry TEntry> inline BindGroupLayout &addEntry(const TEntry &entry)
    {
        this->entries.push_back(std::make_shared<TEntry>(entry));
        return *this;
    }

    const std::vector<std::shared_ptr<IBindGroupLayoutEntry>> &getEntries() const { return this->entries; }

    const std::string &getName() const { return this->name; }

    std::vector<ValidationError> validate(void) const override
    {
        std::vector<ValidationError> errors;
        if (this->entries.empty())
        {
            errors.push_back({"No entries in the bind group layout",
                              fmt::format("BindGroupLayout({})", this->getName()), ValidationError::Severity::Warning});
            return errors;
        }
        for (const auto &entry : this->entries)
        {
            auto entryErrors = entry->validate();
            for (const auto &error : entryErrors)
            {
                errors.push_back({error.message,
                                  fmt::format("BindGroupLayout::{}::{}", error.location, entry->getName()),
                                  error.severity});
            }
        }
        for (uint32_t i = 0; i < this->entries.size(); i++)
        {
            const auto &entry = this->entries[i];
            for (uint32_t j = i + 1; j < this->entries.size(); j++)
            {
                const auto &otherEntry = this->entries[j];
                if (entry->getEntry().binding == otherEntry->getEntry().binding)
                {
                    errors.push_back(
                        {"Binding " + std::to_string(entry->getEntry().binding) + " is duplicated between entries '" +
                             entry->getName() + "' and '" + otherEntry->getName() + "'",
                         fmt::format("BindGroupLayout({})", this->getName()), ValidationError::Severity::Error});
                }
                if (entry->getName() == otherEntry->getName())
                {
                    errors.push_back({"Entry name '" + entry->getName() + "' is duplicated between entries at index " +
                                          std::to_string(i) + " and " + std::to_string(j),
                                      fmt::format("BindGroupLayout({})", this->getName()),
                                      ValidationError::Severity::Warning});
                }
            }
        }
        return errors;
    }

  private:
    std::vector<std::shared_ptr<IBindGroupLayoutEntry>> entries;
    std::string name;
};
} // namespace Graphic::Utils
