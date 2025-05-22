#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace ES::Plugin::Physics {

/**
 * @brief WheeledVehicleBuilderError is an exception class that should be thrown when an error
 * occurs while building a wheeled vehicle.
 *
 * @example "Catching an exception"
 * @code
 * try {
 * } catch (WheeledVehicleBuilderError &e) {
 *   std::cerr << e.what() << std::endl;
 * }
 * @endcode
 *
 * @example "Throwing an exception"
 * @code
 * throw WheeledVehicleBuilderError("Failed to do something");
 * @endcode
 */
class WheeledVehicleBuilderError : public std::exception {
  public:
    explicit WheeledVehicleBuilderError(const std::string &message) : msg("WheeledVehicleBuilder error: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace ES::Plugin::Physics
