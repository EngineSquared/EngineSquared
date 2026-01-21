#pragma once

#include <stdexcept>

namespace Engine::Exception {

class MissingSchedulerError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace Engine::Exception
