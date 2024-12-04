#include <sstream>

#include "String.hpp"

std::vector<std::string> ES::Plugin::Utils::String::Split(const std::string &str, const char &delimiter)
{
    std::vector<std::string> result;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delimiter))
    {
        result.push_back(token);
    }
    return result;
}

bool ES::Plugin::Utils::String::EndsWith(std::string_view str, std::string_view suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}
