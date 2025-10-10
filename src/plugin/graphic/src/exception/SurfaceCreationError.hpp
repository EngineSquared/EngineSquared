#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace Plugin::Graphic::Exception {

class SurfaceCreationError : public std::exception {
  public:
    explicit SurfaceCreationError(const std::string &message) : msg("Surface creation error: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace Plugin::Graphic::Exception
