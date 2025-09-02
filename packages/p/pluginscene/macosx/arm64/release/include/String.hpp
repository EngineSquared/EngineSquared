#include <string>
#include <vector>

namespace ES::Plugin::Utils::String {
/**
 * @brief Split a string by a delimiter
 *
 * @param   str                         The string to split
 * @param   delimiter                   The delimiter to split by
 * @return  std::vector<std::string>    The split string
 */
std::vector<std::string> Split(const std::string &str, const char &delimiter);

/**
 * @brief Check if a string ends with a suffix
 *
 * @param   str                         The string to check
 * @param   suffix                      The suffix to check for
 * @return  bool                        True if the string ends with the suffix, false otherwise
 */
bool EndsWith(std::string_view str, std::string_view suffix);
} // namespace ES::Plugin::Utils::String
