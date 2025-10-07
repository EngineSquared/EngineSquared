#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace Plugin::Object {
class MaterialError : public std::exception {
  public:
    explicit MaterialError(const std::string &message) : msg("Material error: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace Plugin::Object
