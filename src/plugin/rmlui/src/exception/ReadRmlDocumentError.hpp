#pragma once

#include <stdexcept>

namespace Rmlui::Exception {

class ReadRmlDocumentError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

} // namespace Rmlui::Exception
