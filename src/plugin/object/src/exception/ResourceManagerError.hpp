#pragma once

#include <cstring>
#include <stdexcept>
#include <string>

namespace Plugin::Object {

/**
 * @brief ResourceManagerError is an exception class that should be thrown when an error
 * occurs while manipulating ResourceManager objects.
 *
 * @example "Catching an exception"
 * @code
 * try {
 * } catch (ResourceManagerError &e) {
 *   std::cerr << e.what() << std::endl;
 * }
 * @endcode
 *
 * @example "Throwing an exception"
 * @code
 * throw ResourceManagerError("Failed to do something");
 * @endcode
 */
class ResourceManagerError : public std::exception {
  public:
    explicit ResourceManagerError(const std::string &message) : msg("ResourceManager error: " + message){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

} // namespace Plugin::Object
