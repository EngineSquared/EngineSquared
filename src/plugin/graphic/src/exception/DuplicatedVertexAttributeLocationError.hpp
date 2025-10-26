#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace Graphic::Exception {

class DuplicatedVertexAttributeLocationError : public std::exception {
  public:
    explicit DuplicatedVertexAttributeLocationError(const std::string &message)
        : msg("Duplicated vertex attribute location: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace Graphic::Exception
