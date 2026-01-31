/**************************************************************************
 * EngineSquared v0.2.0
 *
 * @file SoftBodyChassisError.hpp
 * @brief Exception type for SoftBodyChassis errors
 *
 * Custom exception to represent errors during SoftBody chassis creation and
 * management. Using a specific exception type makes error handling more
 * precise and allows diagnostic messages to be classified.
 **************************************************************************/

#pragma once

#include <stdexcept>

namespace Physics::Exception {

struct SoftBodyChassisError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace Physics::Exception
