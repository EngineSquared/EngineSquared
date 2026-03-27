#pragma once

#include <stdexcept>

namespace Engine::Exception {
/// @class MissingSchedulerError
/// @brief Exception thrown when a requested scheduler is not found in the Core.
class MissingSchedulerError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace Engine::Exception
