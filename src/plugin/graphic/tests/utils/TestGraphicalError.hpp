#pragma once

#include <stdexcept>

namespace Graphic::Tests::Exception {

class TestGraphicalError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace Graphic::Tests::Exception
