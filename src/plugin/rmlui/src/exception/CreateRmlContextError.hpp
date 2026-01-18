#pragma once

#include <stdexcept>

namespace Rmlui::Exception {

class CreateRmlContextError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace Rmlui::Exception
