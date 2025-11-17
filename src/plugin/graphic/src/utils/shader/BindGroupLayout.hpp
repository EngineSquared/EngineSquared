#pragma once

#include "spdlog/fmt/fmt.h"
#include "utils/IValidable.hpp"
#include "utils/shader/ABindGroupLayoutEntry.hpp"
#include <concepts>
#include <vector>

template <typename TBindGroupLayoutEntry>
concept CBindGroupLayoutEntry = std::derived_from<TBindGroupLayoutEntry, Graphic::Utils::IBindGroupLayoutEntry>;

namespace Graphic::Utils {
class BindGroupLayout : public IValidable<> {
  public:
    explicit BindGroupLayout(const std::string &name) : name(name) {}
    ~BindGroupLayout() override = default;

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
            errors.emplace_back("No entries in the bind group layout",
                                fmt::format("BindGroupLayout({})", this->getName()),
                                ValidationError::Severity::Warning);
            return errors;
        }
        for (const auto &entry : this->entries)
        {
            auto entryErrors = entry->validate();
            for (const auto &error : entryErrors)
            {
                errors.emplace_back(error.message,
                                    fmt::format("BindGroupLayout::{}::{}", error.location, entry->getName()),
                                    error.severity);
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
                    errors.emplace_back(fmt::format("Binding {} is duplicated between entries '{}' and '{}'",
                                                    entry->getEntry().binding, entry->getName(), otherEntry->getName()),
                                        fmt::format("BindGroupLayout({})", this->getName()),
                                        ValidationError::Severity::Error);
                }
                if (entry->getName() == otherEntry->getName())
                {
                    errors.emplace_back(fmt::format("Entry name '{}' is duplicated between entries at index {} and {}",
                                                    entry->getName(), i, j),
                                        fmt::format("BindGroupLayout({})", this->getName()),
                                        ValidationError::Severity::Warning);
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
