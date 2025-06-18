#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace ES::Plugin::UI {

/**
 * @brief UIError is an exception class that should be thrown when an error
 * occurs while manipulating the UI.
 *
 * @example "Catching an exception"
 * @code
 * try {
 * } catch (UIError &e) {
 *   std::cerr << e.what() << std::endl;
 * }
 * @endcode
 *
 * @example "Throwing an exception"
 * @code
 * throw UIError("Failed to do something");
 * @endcode
 */
class UIError : public std::exception {
  public:
    explicit UIError(const std::string &message) : msg("OpenGL error: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace ES::Plugin::UI
