#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace CameraMovement {

/**
 * @brief CameraMovementError is an exception class that should be thrown when an error
 * occurs while manipulating camera movement.
 *
 * @example "Catching an exception"
 * @code
 * try {
 * } catch (CameraMovementError &e) {
 *   std::cerr << e.what() << std::endl;
 * }
 * @endcode
 *
 * @example "Throwing an exception"
 * @code
 * throw CameraMovementError("Failed to do something");
 * @endcode
 */
class CameraMovementError : public std::exception {
  public:
    explicit CameraMovementError(const std::string &message) : msg("CameraMovement error: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace CameraMovement
