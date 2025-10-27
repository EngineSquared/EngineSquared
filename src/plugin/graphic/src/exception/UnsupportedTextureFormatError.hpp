#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace Graphic::Exception {

class UnsupportedTextureFormatError : public std::exception {
  public:
    explicit UnsupportedTextureFormatError(const std::string &message) : msg("Unsupported texture format: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace Graphic::Exception
