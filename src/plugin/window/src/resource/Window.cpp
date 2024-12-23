#include "Window.hpp"

namespace ES::Plugin::Window::Resource {

Window::Window(uint32_t width, uint32_t height, const std::string &title, GLFWmonitor *monitor, GLFWwindow *share)
    : _width(width), _height(height), _title(title), _window(nullptr), _monitor(monitor), _share(share)
{
    Create();
}

Window::~Window()
{
    if (_window)
    {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }
}

void Window::Create()
{
    if (!glfwInit())
        throw ES::Plugin::Window::Exception::WindowError("Failed to initialize GLFW");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    _window = glfwCreateWindow(_width, _height, _title.c_str(), _monitor, _share);

    if (!_window)
    {
        glfwTerminate();
        throw ES::Plugin::Window::Exception::WindowError("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(_window);
}

void Window::GetWindowSize(int &width, int &height)
{
    glfwGetWindowSize(_window, &width, &height);
    _width = width;
    _height = height;
}

void Window::SetFramebufferSizeCallback(void *userPointer, GLFWframebuffersizefun callback)
{
    glfwSetWindowUserPointer(_window, userPointer);
    glfwSetFramebufferSizeCallback(_window, callback);
}

} // namespace ES::Plugin::Window::Resource
