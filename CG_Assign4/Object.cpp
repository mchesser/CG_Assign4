#include "Object.hpp"
#include "GLMUtil.hpp"

#include "glm/geometric.hpp"
#include "glm/mat4x4.hpp"

Object::Object(glm::vec3 position, glm::vec3 direction, glm::vec3 up, glm::vec3 scale)
    : position(position), direction(direction), up(up), scale(scale) {
}

void Object::rotate(glm::vec3 rotation) {
    const glm::vec3 xAxis = glm::cross(direction, up);
    const glm::mat3 rotationMat = glm::mat3(rotate3D(rotation, xAxis, up, direction));

    // Apply the rotation to the object's properties
    up = rotationMat * up;
    direction = rotationMat * direction;
}

void Object::rotateAroundPoint(glm::vec3 rotation, glm::vec3 point) {
    const glm::vec3 pointDir = glm::normalize(position - point);
    const glm::vec3 xAxis = glm::cross(pointDir, up);
    const glm::mat3 rotationMat = glm::mat3(rotate3D(rotation, xAxis, up, pointDir));

    // Apply the rotation to the object's properties
    up = rotationMat * up;
    direction = rotationMat * direction;
    position = point + rotationMat * (position - point);
}

void Object::move(glm::vec3 amount) {
    const glm::vec3 xAxis = glm::cross(direction, up);
    position += amount.x * xAxis + amount.y * up + amount.z * direction;
}

glm::mat4 Object::transformationMatrix() const {
    // Construct a rotation matrix by creating a transformation matrix that transforms the object's
    // axis to the world axis.
    glm::mat4 rotation = glm::mat4(1.0f);
    rotation[0] = glm::vec4(glm::cross(direction, up), 0);
    rotation[1] = glm::vec4(up, 0);
    rotation[2] = glm::vec4(direction, 0);

    return (
        glm::translate(glm::mat4(1.0f), position) *
        rotation *
        glm::scale(glm::mat4(1.0f), scale)
        );
}