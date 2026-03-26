#pragma once

#include <stdexcept>

namespace Physics::Exception {
/// @brief Exception type for errors related to character controllers.
class CharacterControllerError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace Physics::Exception
