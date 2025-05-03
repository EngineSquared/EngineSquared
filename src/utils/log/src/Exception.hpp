#pragma once

#include <cerrno>
#include <cstring>
#include <string>

namespace ES::Utils::Log {
// Platform-safe strerror wrapper
inline std::string GetErrnoMessage()
{
#ifdef _MSC_VER
    std::string buffer(256, '\0');
    strerror_s(buffer.data(), buffer.size(), errno);
    buffer.resize(std::strlen(buffer.c_str()));
    return buffer;
#else
    return std::string(strerror(errno));
#endif
}
} // namespace ES::Utils::Log
