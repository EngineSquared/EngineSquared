#pragma once

#include <ostream>
#include <string>
#include <vector>

namespace Graphic::Utils {

struct ValidationError {
    std::string message;
    std::string location;
    enum class Severity {
        Warning,
        Error
    };
    Severity severity;

    friend inline std::ostream &operator<<(std::ostream &stream, const ValidationError &error)
    {
        stream << "[" << (error.severity == ValidationError::Severity::Error ? "Error" : "Warning") << "] "
               << error.location << ": " << error.message;
        return stream;
    }
};

template <typename... Params>
class IValidable {
  public:
    virtual ~IValidable() = default;
    virtual std::vector<ValidationError> validate(Params... params) const = 0;
};
} // namespace Graphic::Utils
