#include <GLFW/glfw3.h>
#include <iostream>

#if defined(__APPLE__)
#include <AvailabilityMacros.h>
#include <TargetConditionals.h>
#elif defined(__GNUC__) || defined(__clang__)
#include <cxxabi.h>
#include <cstdlib>
#define ATTRIBUTE_WEAK __attribute__((weak))
#else
#define ATTRIBUTE_WEAK
#endif

extern "C" {

ATTRIBUTE_WEAK GLFWwindow *glfwCreateWindow(int, int, const char *, GLFWmonitor *, GLFWwindow *)
{
    return nullptr;
}

ATTRIBUTE_WEAK int glfwInit()
{
    return 1;
}

}
