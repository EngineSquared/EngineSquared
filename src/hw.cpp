#include <iostream>
#include <entt/entt.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include "glm/gtx/string_cast.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

void hw(void)
{
	std::cout << "Hello World!" << std::endl;
	entt::registry e;
	std::cout << (int)e.create() << std::endl;
	glm::vec3 a(1, 2, 3);
	std::cout << glm::to_string(a) << std::endl;
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 42;
	std::cout << bindingDescription.binding << std::endl;
}