#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace Graphic::Exception {

class UnsupportedTextureFormatError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

} // namespace Graphic::Exception
