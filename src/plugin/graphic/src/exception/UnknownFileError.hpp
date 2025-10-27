#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace Graphic::Exception {

class UnknownFileError : public std::exception {
  public:
    explicit UnknownFileError(const std::string &message) : msg("Unknown file error: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace Graphic::Exception
