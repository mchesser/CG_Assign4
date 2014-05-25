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

void Object::lookAt(glm::vec3 point) {
    const glm::vec3 pointDir = glm::normalize(point - position);
    const float dot_product = glm::dot(direction, pointDir);
    if (dot_product >= 1.0f) {
        // If the dot product is greater than or equal to 1, it means the target direction is close 
        // to the current direction. Since floating point errors can affect the values it is
        // possible to get values above 1 causing errors when computing the angle, so we just
        // set the angle directly.
        direction = pointDir;
        return;
    }
    const float angle = glm::acos(dot_product);

    const glm::mat3 rotationMat = glm::mat3(glm::rotate(glm::mat4(1), angle, 
        glm::cross(direction, pointDir)));

    // Apply the rotation to the object's properties
    up = rotationMat * up;
    direction = rotationMat * direction;
}

void Object::move(glm::vec3 amount) {
    const glm::vec3 xAxis = glm::cross(direction, up);
    position += amount.x * xAxis + amount.y * up + amount.z * direction;
}

glm::mat4 Object::transformationMatrix() const {
    return (
        glm::translate(glm::mat4(1.0f), position) *
        rotateBasis(glm::cross(direction, up), up, direction) *
        glm::scale(glm::mat4(1.0f), scale)
        );
}