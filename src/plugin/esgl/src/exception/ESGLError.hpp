#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace ESGL {

/**
 * @brief ESGLError is an exception class that should be thrown when an error
 * occurs while manipulating OpenGL objects.
 *
 * @example "Catching an exception"
 * @code
 * try {
 * } catch (ESGLError &e) {
 *   std::cerr << e.what() << std::endl;
 * }
 * @endcode
 *
 * @example "Throwing an exception"
 * @code
 * throw ESGLError("Failed to do something");
 * @endcode
 */
class ESGLError : public std::exception {
  public:
    explicit ESGLError(const std::string &message) : msg("ESGL error: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace ESGL
