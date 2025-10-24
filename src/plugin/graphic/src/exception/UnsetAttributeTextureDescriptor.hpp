#pragma once

#include <stdexcept>

namespace Graphic::Exception {

class UnsetAttributeTextureDescriptor : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace Graphic::Exception
