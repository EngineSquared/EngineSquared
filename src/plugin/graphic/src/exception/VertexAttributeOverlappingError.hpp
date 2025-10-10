#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace Plugin::Graphic::Exception {

class VertexAttributeOverlappingError : public std::exception {
  public:
    explicit VertexAttributeOverlappingError(const std::string &message) : msg("Vertex attribute overlap: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace Plugin::Graphic::Exception
