#pragma once

#include <stdexcept>

namespace Engine::Exception {

class MissingPluginError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace Engine::Exception
