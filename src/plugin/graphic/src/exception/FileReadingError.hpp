#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace Graphic::Exception {

class FileReadingError : public std::exception {
  public:
    explicit FileReadingError(const std::string &message) : msg("File reading error: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace Graphic::Exception
