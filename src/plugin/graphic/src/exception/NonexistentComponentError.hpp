#pragma once

#include <stdexcept>

namespace Graphic::Exception {

class NonexistentComponentError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace Graphic::Exception
