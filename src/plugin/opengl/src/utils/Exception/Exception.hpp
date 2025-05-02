#pragma once

#include <string>
#include <cstring>
#include <cerrno>

namespace ES::Plugin::OpenGL::Utils
{
    // Platform-safe strerror wrapper
    inline std::string GetErrnoMessage() {
#ifdef _MSC_VER
        std::string buffer(256, '\0');
        strerror_s(buffer.data(), buffer.size(), errno);
        buffer.resize(std::strlen(buffer.c_str()));
        return buffer;
#else
        return std::string(strerror(errno));
#endif
    }
} // namespace ES::Plugin::OpenGL::Utils
