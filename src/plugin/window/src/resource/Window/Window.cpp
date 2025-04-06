#include "Window.hpp"
#include "Logger.hpp"

namespace ES::Plugin::Window::Resource {

Window::Window(uint32_t width, uint32_t height, const std::string &title, GLFWmonitor *monitor, GLFWwindow *share)
    : _width(width), _height(height), _title(title), _window(nullptr), _monitor(monitor), _share(share)
{
    _window = glfwCreateWindow(_width, _height, _title.c_str(), _monitor, _share);
#ifdef ES_DEBUG
    if (!_window)
        ES::Utils::Error("Failed to create window");
#endif
}

void Window::Destroy()
{
    if (!_window)
        throw ES::Plugin::Window::Exception::WindowError("Window is not created");

    /* To use in a shutdown scheduler */
    glfwDestroyWindow(_window);
}
//TODO: rename this to GetSize or Size (and remove parameters)
void Window::GetWindowSize(int &width, int &height)
{
    if (!_window)
        throw ES::Plugin::Window::Exception::WindowError("Window is not created");

    glfwGetWindowSize(_window, &width, &height);
    _width = width;
    _height = height;
}

void Window::SetFramebufferSizeCallback(void *userPointer, GLFWframebuffersizefun callback)
{
    if (!_window)
        throw ES::Plugin::Window::Exception::WindowError("Window is not created");

    glfwSetWindowUserPointer(_window, userPointer);
    glfwSetFramebufferSizeCallback(_window, callback);
}

} // namespace ES::Plugin::Window::Resource
