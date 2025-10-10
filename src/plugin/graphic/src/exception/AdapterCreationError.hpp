#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace Plugin::Graphic::Exception {

class AdapterCreationError : public std::exception {
  public:
    explicit AdapterCreationError(const std::string &message) : msg("Adapter creation error: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace Plugin::Graphic::Exception
