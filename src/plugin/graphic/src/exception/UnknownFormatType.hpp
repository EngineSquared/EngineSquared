#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace Graphic::Exception {

class UnknownFormatType : public std::exception {
  public:
    explicit UnknownFormatType(const std::string &message) : msg("Unknown format type: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace Graphic::Exception
