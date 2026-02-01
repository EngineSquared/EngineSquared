#pragma once

#include <cstdlib>
#include <memory>
#include <string>
#include <typeinfo>
#if defined(__GNUG__) || defined(__clang__)
#    include <cxxabi.h>
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
    if (auto ptr = std::unique_ptr<char, void (*)(void *)>(
            abi::__cxa_demangle(typeInfo.name(), nullptr, nullptr, &status), [](void *p) { std::free(p); });
        status == 0 && ptr)
        return std::string(ptr.get());
#endif
    return typeInfo.name();
}

} // namespace FunctionUtils
