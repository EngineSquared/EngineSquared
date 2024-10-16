#include "Window.hpp"

ES::Plugin::Window::Resource::Window::Window(uint32_t width, uint32_t height, const std::string &title,
                                             GLFWmonitor *monitor, GLFWwindow *share)
    : _width(width), _height(height), _title(title), _monitor(monitor), _share(share), _window(nullptr)
{
    Create();
}

ES::Plugin::Window::Resource::Window::~Window()
{
    if (_window)
    {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }
}

void ES::Plugin::Window::Resource::Window::Create()
{
    if (!glfwInit())
    {
        throw ES::Plugin::Window::Exception::WindowError("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window = glfwCreateWindow(_width, _height, _title.c_str(), _monitor, _share);

    if (!_window)
    {
        glfwTerminate();
        throw ES::Plugin::Window::Exception::WindowError("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(_window);
}
