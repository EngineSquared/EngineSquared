#pragma once

#include <stdexcept>
#include <string>

namespace ES::Plugin::Window::Exception {

/**
 * WindowError is an exception class that should be thrown when an error occurs while creating or destroying a window.
 */
class WindowError : public std::runtime_error {
  public:
    explicit WindowError(const std::string &message) : std::runtime_error("Window error: " + message) {}
};

} // namespace ES::Plugin::Window::Exception
