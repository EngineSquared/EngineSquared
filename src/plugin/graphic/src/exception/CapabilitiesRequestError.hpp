#pragma once

#include <stdexcept>

namespace Plugin::Graphic::Exception {

class CapabilitiesRequestError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace Plugin::Graphic::Exception
