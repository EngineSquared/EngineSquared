#pragma once

#include <glm/glm.hpp>

namespace ES::Plugin::OpenGL::Resource {
class DirectionalLight
{
public:
	bool enabled = true;
	GLuint depthMapFBO = 0;
	GLuint depthMap = 0;
	GLsizei SHADOW_WIDTH = 1280;
	GLsizei SHADOW_HEIGHT = 720;

	glm::vec3 posOfLight = glm::vec3(6.f, 20.f, 6.f);
	float near_plane = 1.0f;
	float far_plane = 50.0f;
	glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, 20.0f, -20.0f, 1.0f, 50.0f);
	glm::mat4 lightView = glm::lookAt(glm::vec3(6.f, 20.f, 6.f), 
									  glm::vec3(0.0f, 5.0f, 0.0f), 
									  glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightSpaceMatrix = lightProjection * lightView;
};
} // namespace ES::Plugin::OpenGL::Resource
