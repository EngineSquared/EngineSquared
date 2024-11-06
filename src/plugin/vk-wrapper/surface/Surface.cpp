#include "Surface.hpp"

namespace ES::Plugin::Wrapper {

void Surface::Create(GLFWwindow *window, const VkInstance &instance)
{
    if (glfwCreateWindowSurface(instance, window, nullptr, &_surface) != VK_SUCCESS)
        throw VkWrapperError("Failed to create window surface!");
}

void Surface::Destroy(const VkInstance &instance) { vkDestroySurfaceKHR(instance, _surface, nullptr); }

} // namespace ES::Plugin::Wrapper
