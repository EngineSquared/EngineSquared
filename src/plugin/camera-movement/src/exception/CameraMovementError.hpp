#pragma once

#include <stdexcept>

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
class CameraMovementError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace CameraMovement
