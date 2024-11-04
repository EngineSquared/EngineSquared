/**************************************************************************
 * VkWrapper v0.0.0
 *
 * VkWrapper is a software package, part of the Engine².
 *
 * This file is part of the VkWrapper project that is under GPL-3.0 License.
 * Copyright © 2024 by @EngineSquared, All rights reserved.
 *
 * VkWrapper is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file config.h
 * @brief Compile-Time Configuration Parameters for VkWrapper.
 *
 * @author @MasterLaplace
 * @version 0.0.0
 * @date 2024-10-15
 **************************************************************************/

// clang-format off
#ifndef CONFIG_H_
    #define CONFIG_H_

#ifdef __cplusplus
    #include <utility>
    #include <type_traits>

    #include <cstddef>
    #include <cstdint>
#else
    #include <stddef.h>
    #include <stdint.h>
#endif

#ifndef DISTRIBUTION_H_
    #define DISTRIBUTION_H_

////////////////////////////////////////////////////////////
// Identify the Compiler
////////////////////////////////////////////////////////////
#if defined(_MSC_VER) || defined(_MSVC_LANG)
    #define VKWRAPPER_COMPILER_MSVC
    #define VKWRAPPER_COMPILER_STRING "MSVC"
#elif defined(__GNUC__) || defined(__GNUG__)
    #define VKWRAPPER_COMPILER_GCC
    #define VKWRAPPER_COMPILER_STRING "GCC"
#elif defined(__clang__) || defined(__llvm__)
    #define VKWRAPPER_COMPILER_CLANG
    #define VKWRAPPER_COMPILER_STRING "Clang"
#elif defined(__MINGW32__) || defined(__MINGW64__)
    #define VKWRAPPER_COMPILER_MINGW
    #define VKWRAPPER_COMPILER_STRING "MinGW"
#elif defined(__CYGWIN__)
    #define VKWRAPPER_COMPILER_CYGWIN
    #define VKWRAPPER_COMPILER_STRING "Cygwin"
#else
    #error [Config@Distribution]: This compiler is not supported by VKWRAPPER library.
#endif


////////////////////////////////////////////////////////////
// Identify the Operating System
////////////////////////////////////////////////////////////
#if defined(_WIN32) || defined(__WIN32__) || defined(VKWRAPPER_COMPILER_MINGW) || defined(VKWRAPPER_COMPILER_CYGWIN)

    #define VKWRAPPER_SYSTEM_WINDOWS
    #define VKWRAPPER_SYSTEM_STRING "Windows"

// Android is based on the Linux VKWRAPPER, so it has to appear before Linux
#elif defined(__ANDROID__)

    #define VKWRAPPER_SYSTEM_ANDROID
    #define VKWRAPPER_SYSTEM_STRING "Android"

#elif defined(linux) || defined(__linux)

    #define VKWRAPPER_SYSTEM_LINUX
    #define VKWRAPPER_SYSTEM_STRING "Linux"

#elif defined(__unix) || defined(__unix__)

    #define VKWRAPPER_SYSTEM_UNIX
    #define VKWRAPPER_SYSTEM_STRING "Unix"

#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)

    #define VKWRAPPER_SYSTEM_MACOS
    #define VKWRAPPER_SYSTEM_STRING "MacOS"

#elif defined(__FreeBSD__) || defined(__FreeBSD_VKWRAPPER__)

    #define VKWRAPPER_SYSTEM_FREEBSD
    #define VKWRAPPER_SYSTEM_STRING "FreeBSD"

#elif defined(LAPLACE_KERNEL_PANIC)

    #define VKWRAPPER_SYSTEM_KERNEL
    #define VKWRAPPER_SYSTEM_STRING "Laplace Kernel"

#else
    #error [Config@Distribution]: This operating system is not supported by VKWRAPPER library.
#endif

#ifdef __cplusplus
    #define VKWRAPPER_EXTERN_C extern "C"

    #if __cplusplus >= 202203L
        #define VKWRAPPER_CPP23(_) _
        #define VKWRAPPER_CPP20(_) _
        #define VKWRAPPER_CPP17(_) _
        #define VKWRAPPER_CPP14(_) _
        #define VKWRAPPER_CPP11(_) _
        #define VKWRAPPER_CPP99(_) _
    #elif __cplusplus >= 202002L
        #define VKWRAPPER_CPP23(_)
        #define VKWRAPPER_CPP20(_) _
        #define VKWRAPPER_CPP17(_) _
        #define VKWRAPPER_CPP14(_) _
        #define VKWRAPPER_CPP11(_) _
        #define VKWRAPPER_CPP99(_) _
    #elif __cplusplus >= 201703L
        #define VKWRAPPER_CPP23(_)
        #define VKWRAPPER_CPP20(_)
        #define VKWRAPPER_CPP17(_) _
        #define VKWRAPPER_CPP14(_) _
        #define VKWRAPPER_CPP11(_) _
        #define VKWRAPPER_CPP99(_) _
    #elif __cplusplus >= 201402L
        #define VKWRAPPER_CPP23(_)
        #define VKWRAPPER_CPP20(_)
        #define VKWRAPPER_CPP17(_)
        #define VKWRAPPER_CPP14(_) _
        #define VKWRAPPER_CPP11(_) _
        #define VKWRAPPER_CPP99(_) _
    #elif __cplusplus >= 201103L
        #define VKWRAPPER_CPP23(_)
        #define VKWRAPPER_CPP20(_)
        #define VKWRAPPER_CPP17(_)
        #define VKWRAPPER_CPP14(_)
        #define VKWRAPPER_CPP11(_) _
        #define VKWRAPPER_CPP99(_) _
    #elif __cplusplus >= 199711L
        #define VKWRAPPER_CPP23(_)
        #define VKWRAPPER_CPP20(_)
        #define VKWRAPPER_CPP17(_)
        #define VKWRAPPER_CPP14(_)
        #define VKWRAPPER_CPP11(_)
        #define VKWRAPPER_CPP99(_) _
    #else
        #define VKWRAPPER_CPP23(_)
        #define VKWRAPPER_CPP20(_)
        #define VKWRAPPER_CPP17(_)
        #define VKWRAPPER_CPP14(_)
        #define VKWRAPPER_CPP11(_)
        #define VKWRAPPER_CPP99(_)
    #endif

    ////////////////////////////////////////////////////////////
    // Define a macro to handle cpp features compatibility
    ////////////////////////////////////////////////////////////
    /** Usage:
     * @example
     * void func() VKWRAPPER_CPP14([[deprecated]]);
     *
     * @example
     * void func() VKWRAPPER_CPP([[deprecated]], 14);
    \**********************************************************/
    #define VKWRAPPER_CPP(_, version) VKWRAPPER_CPP##version(_)

#else
    #define VKWRAPPER_EXTERN_C extern

    #define VKWRAPPER_CPP23(_)
    #define VKWRAPPER_CPP20(_)
    #define VKWRAPPER_CPP17(_)
    #define VKWRAPPER_CPP14(_)
    #define VKWRAPPER_CPP11(_)
    #define VKWRAPPER_CPP99(_)
    #define VKWRAPPER_CPP(_, version)
#endif

////////////////////////////////////////////////////////////
// Define helpers to create portable import / export macros for each module
////////////////////////////////////////////////////////////
#if defined(VKWRAPPER_SYSTEM_WINDOWS)

    // Windows compilers need specific (and different) keywords for export and import
    #define VKWRAPPER_API_EXPORT extern "C" __declspec(dllexport)
    #define VKWRAPPER_API_IMPORT VKWRAPPER_EXTERN_C __declspec(dllimport)

    // For Visual C++ compilers, we also need to turn off this annoying C4251 warning
    #ifdef _MSC_VER

        #pragma warning(disable : 4251)

    #endif

#else // Linux, FreeBSD, Mac OS X

    #if __GNUC__ >= 4

        // GCC 4 has special keywords for showing/hidding symbols,
        // the same keyword is used for both importing and exporting
        #define VKWRAPPER_API_EXPORT extern "C" __attribute__ ((__visibility__ ("default")))
        #define VKWRAPPER_API_IMPORT VKWRAPPER_EXTERN_C __attribute__ ((__visibility__ ("default")))

    #else

        // GCC < 4 has no mechanism to explicitely hide symbols, everything's exported
        #define VKWRAPPER_API_EXPORT extern "C"
        #define VKWRAPPER_API_IMPORT VKWRAPPER_EXTERN_C

    #endif

#endif


#ifdef VKWRAPPER_SYSTEM_WINDOWS

    // Windows compilers use a different name for the main function
    #define VKWRAPPER_GUI_MAIN(hInstance, hPrevInstance, lpCmdLine, nCmdShow) WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
    #define VKWRAPPER_MAIN(ac, av, env) main(int ac, char *av[], char *env[])

#elif defined(VKWRAPPER_SYSTEM_ANDROID)

    // Android doesn't need a main function
    #define VKWRAPPER_GUI_MAIN(app) android_main(struct android_app* app)
    #define VKWRAPPER_MAIN

#elif defined(VKWRAPPER_SYSTEM_MACOS)

    // On MacOS X, we use a Unix main function
    #define VKWRAPPER_MAIN(ac, av, env, apple) main(int ac, char *av[], char *env[], char *apple[])

#else

    // Other platforms should use the standard main function
    #define VKWRAPPER_MAIN(ac, av, env) main(int ac, char *av[], char *env[])
#endif

////////////////////////////////////////////////////////////
// Define portable NULL pointer using C++11 nullptr keyword
////////////////////////////////////////////////////////////
#ifdef __cplusplus
#elif !defined(NULL)
    #define nullptr ((void*)0)
#else
    #define nullptr NULL
#endif

////////////////////////////////////////////////////////////
// Define boolean type and values
////////////////////////////////////////////////////////////
#if defined(__cplusplus)
#elif !defined(__bool_true_false_are_defined)
    #define bool _Bool
    #define true 1
    #define false 0
    #define __bool_true_false_are_defined 1
#endif

////////////////////////////////////////////////////////////
// Define a portable debug macro
////////////////////////////////////////////////////////////
#if (defined(_DEBUG) || defined(DEBUG)) && !defined(NDEBUG)

    #define VKWRAPPER_DEBUG
    #define VKWRAPPER_DEBUG_STRING "Debug"

#else
    #define VKWRAPPER_DEBUG_STRING "Release"
#endif


#if defined __GNUC__ && defined __GNUC_MINOR__
# define __GNUC_PREREQ(maj, min) \
    ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
# define __GNUC_PREREQ(maj, min) 0
#endif

////////////////////////////////////////////////////////////
// Define a portable way to declare a function as deprecated
////////////////////////////////////////////////////////////
/** Usage:
 * @example "for functions"
 *   VKWRAPPER_DEPRECATED void func();
 * @example "for structs"
 *   struct VKWRAPPER_DEPRECATED MyStruct { ... };
 * @example "for enums"
 *   enum VKWRAPPER_DEPRECATED MyEnum { ... };
 *   enum MyEnum {
 *        MyEnum1 = 0,
 *        MyEnum2 VKWRAPPER_DEPRECATED,
 *        MyEnum3
 *   };
 * @example "for classes"
 *   class VKWRAPPER_DEPRECATED MyClass { ... };
\**********************************************************/
#ifdef VKWRAPPER_DISABLE_DEPRECATION

    #define VKWRAPPER_DEPRECATED
    #define VKWRAPPER_DEPRECATED_MSG(message)
    #define VKWRAPPER_DEPRECATED_VMSG(version, message)

#elif defined(__cplusplus) && (__cplusplus >= 201402)

    #define VKWRAPPER_DEPRECATED [[deprecated]]

    #if (__cplusplus >= 201402) && (__cplusplus < 201703)

        #define VKWRAPPER_DEPRECATED_MSG(message) [[deprecated(message)]]
        #define VKWRAPPER_DEPRECATED_VMSG(version, message) \
            [[deprecated("since " # version ". " message)]]

    #else
        #define VKWRAPPER_DEPRECATED_MSG(message) [[deprecated]]
        #define VKWRAPPER_DEPRECATED_VMSG(version, message) [[deprecated]]
    #endif

#elif defined(VKWRAPPER_COMPILER_MSVC) && (_MSC_VER >= 1400)

    #define VKWRAPPER_DEPRECATED __declspec(deprecated)

    #if (_MSC_VER >= 1900)

        #define VKWRAPPER_DEPRECATED_MSG(message) __declspec(deprecated(message))
        #define VKWRAPPER_DEPRECATED_VMSG(version, message) \
            __declspec(deprecated("since " # version ". " message))

    #else
        #define VKWRAPPER_DEPRECATED_MSG(message) __declspec(deprecated)
        #define VKWRAPPER_DEPRECATED_VMSG(version, message) __declspec(deprecated)
    #endif

#elif defined(VKWRAPPER_COMPILER_CLANG) && defined(__has_feature)

    #define VKWRAPPER_DEPRECATED __attribute__((deprecated))

    #if __has_feature(attribute_deprecated_with_message)

        #define VKWRAPPER_DEPRECATED_MSG(message) __attribute__((deprecated(message)))
        #define VKWRAPPER_DEPRECATED_VMSG(version, message) \
            __attribute__((deprecated("since " # version ". " message)))

    #else
        #define VKWRAPPER_DEPRECATED_MSG(message) __attribute__((deprecated))
        #define VKWRAPPER_DEPRECATED_VMSG(version, message) __attribute__((deprecated))
    #endif

#elif defined(VKWRAPPER_COMPILER_GCC) && defined(__GNUC__) && __GNUC_PREREQ(4, 5)

    #define VKWRAPPER_DEPRECATED __attribute__((deprecated))

    #if defined(VKWRAPPER_COMPILER_GCC) && defined(__GNUC__) && __GNUC_PREREQ(4, 9)

        #define VKWRAPPER_DEPRECATED_MSG(message) __attribute__((deprecated(message)))
        #define VKWRAPPER_DEPRECATED_VMSG(version, message) \
            __attribute__((deprecated("since " # version ". " message)))

    #else
        #define VKWRAPPER_DEPRECATED_MSG(message) __attribute__((deprecated))
        #define VKWRAPPER_DEPRECATED_VMSG(version, message) __attribute__((deprecated))
    #endif

#else

    #pragma message("WARNING: VKWRAPPER_DEPRECATED not supported on this compiler")
    #define VKWRAPPER_DEPRECATED
    #define VKWRAPPER_DEPRECATED_MSG(message)
    #define VKWRAPPER_DEPRECATED_VMSG(version, message)
#endif


////////////////////////////////////////////////////////////
// Define a portable way for packing structures
////////////////////////////////////////////////////////////
/** Usage:
 * @example
 * PACKED(struct MyStruct
 * {
 *     int a;
 *     char b;
 *     ...
 * });
\**********************************************************/
#if defined(__GNUC__) || defined(__GNUG__)
    #define PACKED( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#elif _MSC_VER
    #define PACKED( __Declaration__ ) __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#else
    #define PACKED( __Declaration__ ) __Declaration__
#endif

#endif /* !DISTRIBUTION_H_ */


#ifndef VERSION_H_
    #define VERSION_H_

////////////////////////////////////////////////////////////
// Define the VKWRAPPER version
////////////////////////////////////////////////////////////
#ifdef FLAG_VERSION_MAJOR
    #define VKWRAPPER_VERSION_MAJOR FLAG_VERSION_MAJOR
#else
    #define VKWRAPPER_VERSION_MAJOR 0
#endif

#ifdef FLAG_VERSION_MINOR
    #define VKWRAPPER_VERSION_MINOR FLAG_VERSION_MINOR
#else
    #define VKWRAPPER_VERSION_MINOR 0
#endif

#ifdef FLAG_VERSION_PATCH
    #define VKWRAPPER_VERSION_PATCH FLAG_VERSION_PATCH
#else
    #define VKWRAPPER_VERSION_PATCH 0
#endif

#ifdef FLAG_VERSION_TWEAK
    #define VKWRAPPER_VERSION_TWEAK FLAG_VERSION_TWEAK
#else
    #define VKWRAPPER_VERSION_TWEAK 0
#endif

////////////////////////////////////////////////////////////
// Define the VKWRAPPER version number
////////////////////////////////////////////////////////////
#define VKWRAPPER_VERSION \
        (VKWRAPPER_VERSION_MAJOR * 1000000 + \
        VKWRAPPER_VERSION_MINOR * 10000 + \
        VKWRAPPER_VERSION_PATCH * 100 + \
        VKWRAPPER_VERSION_TWEAK)

#define VKWRAPPER_PREREQ_VERSION(maj, min, pat) (VKWRAPPER_VERSION >= (maj * 1000000 + min * 10000 + pat * 100))

////////////////////////////////////////////////////////////
// Define the VKWRAPPER version concatenated
////////////////////////////////////////////////////////////
#define VKWRAPPER_VERSION_CCT VKWRAPPER_VERSION_MAJOR##_##VKWRAPPER_VERSION_MINOR##_##VKWRAPPER_VERSION_PATCH##_##VKWRAPPER_VERSION_TWEAK


////////////////////////////////////////////////////////////
// Define the VKWRAPPER version string
////////////////////////////////////////////////////////////
// Helper macro to convert a macro to a string
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define VKWRAPPER_VERSION_STRING \
        TOSTRING(VKWRAPPER_VERSION_MAJOR) "." \
        TOSTRING(VKWRAPPER_VERSION_MINOR) "." \
        TOSTRING(VKWRAPPER_VERSION_PATCH) "." \
        TOSTRING(VKWRAPPER_VERSION_TWEAK)

#endif /* !VERSION_H_ */


////////////////////////////////////////////////////////////
// Compile-Time Configuration Parameters
////////////////////////////////////////////////////////////
#define VKWRAPPER_CONFIG_STRING \
        "VKWRAPPER_VERSION=" VKWRAPPER_VERSION_STRING "\n" \
        "VKWRAPPER_SYSTEM=" VKWRAPPER_SYSTEM_STRING "\n" \
        "VKWRAPPER_COMPILER=" VKWRAPPER_COMPILER_STRING "\n" \
        "VKWRAPPER_DEBUG=" VKWRAPPER_DEBUG_STRING "\n"

#endif /* !CONFIG_H_ */
// clang-format on
