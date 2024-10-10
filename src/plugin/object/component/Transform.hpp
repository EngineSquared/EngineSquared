#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

namespace ES::Plugin::Object::Component{
	struct Transform {
		glm::vec3 position;
		glm::vec3 scale;
        glm::quat rotation;

		Transform(glm::vec3 position = glm::vec3(0), glm::vec3 scale = glm::vec3(1), glm::quat rotation = glm::quat(1, 0, 0, 0)) :
            position(position), scale(scale), rotation(rotation) {}

        glm::vec3 getPosition() const { return position; }
        glm::vec3 getScale() const { return scale; }
        glm::quat getRotation() const { return rotation; }

        // Setters
        void setPosition(const glm::vec3& newPosition) { position = newPosition; }
		void setPosition(float x, float y, float z) { position = glm::vec3(x, y, z); }
        void setScale(const glm::vec3& newScale) { scale = newScale; }
		void setScale(float x, float y, float z) { scale = glm::vec3(x, y, z); }
        void setRotation(const glm::quat& newRotation) { rotation = newRotation; }
		void setRotation(float x, float y, float z, float w) { rotation = glm::quat(w, x, y, z); }

        // Utility methods
        glm::mat4 getTransformationMatrix() const {
            glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
			glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
            glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
            return translation * rotationMatrix * scaleMatrix;
        }

        // Static methods (unrelated to instance)
        static Transform identity() {
            return Transform{
                glm::vec3(0.0f),    // Position
                glm::vec3(1.0f),    // Scale
                glm::quat(1.0f, 0.0f, 0.0f, 0.0f) // Rotation as a quaternion
            };
        }
    };
}
