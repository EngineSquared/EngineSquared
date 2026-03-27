#pragma once

#include <stdexcept>

namespace Engine::Exception {
/// @class MissingResourceError
/// @brief Exception thrown when a requested resource is not found in the Core.
class MissingResourceError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace Engine::Exception
