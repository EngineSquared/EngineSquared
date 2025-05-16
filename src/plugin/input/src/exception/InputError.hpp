#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace ES::Plugin::Input {

/**
 * @brief InputError is an exception class that should be thrown when an error
 * occurs while manipulating OpenGL objects.
 *
 * @example "Catching an exception"
 * @code
 * try {
 * } catch (InputError &e) {
 *   std::cerr << e.what() << std::endl;
 * }
 * @endcode
 *
 * @example "Throwing an exception"
 * @code
 * throw InputError("Failed to do something");
 * @endcode
 */
class InputError : public std::exception {
  public:
    explicit InputError(const std::string &message) : msg("Input error: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace ES::Plugin::Input
