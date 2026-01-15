#pragma once

#include <stdexcept>

namespace Physics::Exception {

class VehicleBuilderError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace Physics::Exception
