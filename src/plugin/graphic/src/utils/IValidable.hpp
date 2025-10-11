#pragma once

#include <ostream>
#include <string>
#include <vector>

namespace Plugin::Graphic::Utils {

struct ValidationError {
    std::string message;
    std::string location;
    enum class Severity {
        Warning,
        Error
    };
    Severity severity;
};

std::ostream &operator<<(std::ostream &stream, const ValidationError &error)
{
    stream << "[" << (error.severity == ValidationError::Severity::Error ? "Error" : "Warning") << "] "
           << error.location << ": " << error.message;
    return stream;
}

class IValidable {
  public:
    virtual ~IValidable() = default;
    virtual std::vector<ValidationError> validate() const = 0;
};
} // namespace Plugin::Graphic::Utils
