#pragma once

#include <cstdlib>
#include <cxxabi.h>
#include <string>
#include <typeinfo>

namespace FunctionUtils {

/**
 * @brief Démangle un nom de type C++ (typeid) en une chaîne lisible.
 * @param typeInfo L'objet type_info à démangler.
 * @return Le nom démanglé si possible, sinon le nom brut.
 */
inline std::string DemangleTypeName(const std::type_info &typeInfo)
{
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
        return typeInfo.name();
    }
}

} // namespace FunctionUtils
