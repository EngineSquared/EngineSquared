#pragma once

#include <cerrno>
#include <cstring>
#include <string>

namespace ES::Plugin::OpenGL::Utils {
// Platform-safe strerror wrapper
inline std::string GetErrnoMessage()
{
#ifdef _MSC_VER
    char buffer[256];
    strerror_s(buffer, sizeof(buffer), errno);
    return std::string(buffer);
#else
    return std::string(strerror(errno));
#endif
}
} // namespace ES::Plugin::OpenGL::Utils
