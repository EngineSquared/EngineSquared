#pragma once

#include <cstdlib>
#if defined(__GNUG__) || defined(__clang__)
#    include <cxxabi.h>
#    include <string>
#    include <typeinfo>
#endif

namespace FunctionUtils {

/**
 * @brief Démangle un nom de type C++ (typeid) en une chaîne lisible.
 * @param typeInfo L'objet type_info à démangler.
 * @return Le nom démanglé si possible, sinon le nom brut.
 */
inline std::string DemangleTypeName(const std::type_info &typeInfo)
{
#if defined(__GNUG__) || defined(__clang__)
    int status = 0;
    char *demangledName = abi::__cxa_demangle(typeInfo.name(), nullptr, nullptr, &status);
    if (status == 0 && demangledName != nullptr)
    {
        std::string name = demangledName;
        std::free(demangledName);
        return name;
    }
    else
    {
        if (demangledName != nullptr)
        {
            std::free(demangledName);
        }
    }
#endif
    return typeInfo.name();
}

} // namespace FunctionUtils
