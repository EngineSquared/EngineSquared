#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

#include <glm/gtx/string_cast.hpp>
#include <iostream>

namespace Object::Component {
/**
 * Component used to represent a 3D transformation for a game object.
 * It can be used as a 2D transformation.
 * This transformation is represented by position, scale, and rotation.
 */
struct Transform {
    /**
     * \param   position    position of the entity. Default is (0, 0, 0).
     * \param   scale       scale of the entity. Default is (1, 1, 1).
     * \param   rotation    rotation of the entity. Default is identity (no rotation).
     */
    Transform(glm::vec3 position = glm::vec3(0), glm::vec3 scale = glm::vec3(1),
              glm::quat rotation = glm::quat(1, 0, 0, 0))
        : _position(position), _scale(scale), _rotation(rotation)
    {
    }

    Transform(const Transform &) = default;
    Transform(Transform &&) = default;

    Transform &operator=(const Transform &) = default;
    Transform &operator=(Transform &&) = default;

    // Getters
    inline const glm::vec3 &GetPosition() const { return _position; }
    inline const glm::vec3 &GetScale() const { return _scale; }
    inline const glm::quat &GetRotation() const { return _rotation; }

    // Setters
    void SetPosition(const glm::vec3 &newPosition)
    {
        _dirty = true;
        _position = newPosition;
    }
    void SetPosition(float x, float y, float z)
    {
        _dirty = true;
        _position = glm::vec3(x, y, z);
    }
    void SetScale(const glm::vec3 &newScale)
    {
        _dirty = true;
        _scale = newScale;
    }
    void SetScale(float x, float y, float z)
    {
        _dirty = true;
        _scale = glm::vec3(x, y, z);
    }
    void SetRotation(const glm::quat &newRotation)
    {
        _dirty = true;
        _rotation = newRotation;
    }
    void SetRotation(float x, float y, float z, float w)
    {
        _dirty = true;
        _rotation = glm::quat(w, x, y, z);
    }

    glm::vec3 GetForwardVector() const
    {
        glm::vec3 direction;
        direction.x = 2.0f * (_rotation.x * _rotation.z - _rotation.w * _rotation.y);
        direction.y = 2.0f * (_rotation.w * _rotation.x + _rotation.y * _rotation.z);
        direction.z = 1.0f - 2.0f * (_rotation.x * _rotation.x + _rotation.y * _rotation.y);
        return glm::normalize(direction);
    }
    /**
     * Create the transformation matrix for this transform component.
     *
     * \return  transformation matrix that combines the position, scale, and rotation of the entity.
     *
     */
    glm::mat4 ComputeTransformationMatrix()
    {
        if (_dirty)
        {
            _transformationMatrixCache = _BuildTransformationMatrix();
            _dirty = false;
        }
        return _transformationMatrixCache;
    }

  private:
    /**
     * Position of the entity
     */
    glm::vec3 _position;
    /**
     * Scale of the entity
     */
    glm::vec3 _scale;
    /**
     * Rotation of the entity
     * Represents a rotation around the origin, represented as a quaternion.
     */
    glm::quat _rotation;

    bool _dirty = true;
    glm::mat4 _transformationMatrixCache = glm::mat4(1.0f);

    inline glm::mat4 _BuildTransformationMatrix() const
    {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), _position);
        glm::mat4 rotationMatrix = glm::mat4_cast(_rotation);
        glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), _scale);
        return translation * rotationMatrix * scaleMatrix;
    }
};
} // namespace Object::Component
