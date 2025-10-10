#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace Plugin::Physics {

/**
 * @brief ContactListenerError is an exception class that should be thrown when an error
 * occurs while manipulating contact callbacks.
 *
 * @example "Catching an exception"
 * @code
 * try {
 * } catch (ContactListenerError &e) {
 *   std::cerr << e.what() << std::endl;
 * }
 * @endcode
 *
 * @example "Throwing an exception"
 * @code
 * throw ContactListenerError("Failed to do something");
 * @endcode
 */
class ContactListenerError : public std::exception {
  public:
    explicit ContactListenerError(const std::string &message) : msg("ContactCallback error: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace Plugin::Physics
