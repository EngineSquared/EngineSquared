#include <sstream>
#include <string>
#include <vector>

namespace ES::Plugin::Utils::String {
std::vector<std::string> Split(const std::string &str, const char &delimiter)
{
    std::vector<std::string> result;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter))
    {
        result.push_back(token);
    }
    return std::move(result);
}

bool EndsWith(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}
} // namespace ES::Plugin::Utils::String
