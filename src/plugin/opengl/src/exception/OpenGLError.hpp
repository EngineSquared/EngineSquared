#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace ES::Plugin::OpenGL {

/**
 * @brief OpenGLError is an exception class that should be thrown when an error
 * occurs while manipulating OpenGL objects.
 *
 * @example "Catching an exception"
 * @code
 * try {
 * } catch (OpenGLError &e) {
 *   std::cerr << e.what() << std::endl;
 * }
 * @endcode
 *
 * @example "Throwing an exception"
 * @code
 * throw OpenGLError("Failed to do something");
 * @endcode
 */
class OpenGLError : public std::exception {
  public:
    explicit OpenGLError(const std::string &message) : msg("OpenGL error: " + message) {};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace ES::Plugin::OpenGL
