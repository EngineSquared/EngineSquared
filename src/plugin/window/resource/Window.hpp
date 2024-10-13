#pragma once

#include <string>

#include <GLFW/glfw3.h>
#include <stdexcept>

// TODO: remove relative path include
#include "../exception/WindowError.hpp"

namespace ES::Plugin::Window::Resource {
    /**
     * The Window class is a singleton to handle the creation and destruction of the game's window.
     */
    class Window
    {
        private:
            int _width;
            int _height;
            std::string _title;
            GLFWwindow* _window;
            GLFWmonitor* _monitor;
            GLFWwindow* _share;

        public:
            Window(int width, int height, const std::string& title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr)
                : _width(width), _height(height), _title(title), _monitor(monitor), _share(share)
            {
            }

            void Create()
            {
                if (!glfwInit())
                {
                    throw ES::Plugin::Window::Exception::WindowError("Failed to initialize GLFW");
                }

                _window = glfwCreateWindow(_width, _height, _title.c_str(), _monitor, _share);

                if (!_window)
                {
                    glfwTerminate();
                    throw ES::Plugin::Window::Exception::WindowError("Failed to create GLFW window");
                }

                glfwMakeContextCurrent(_window);
            }

            inline bool ShouldClose() const
            {
                return glfwWindowShouldClose(_window);
            }

            inline GLFWwindow* GetWindow() const
            {
                return _window;
            }

            ~Window()
            {
                if (_window)
                {
                    glfwDestroyWindow(_window);
                    glfwTerminate();
                }
            }
    };
}